from scipy.linalg import cho_factor, cho_solve, solve_triangular, cholesky, norm
from scipy.sparse import csr_matrix, csc_matrix
import numpy as np


class LFM:
    def __init__(self, n_components, lamb=1e-2, mu=1e-2):
        """
        Parameters:
        -----------
            n_components : float, number of components in Latent Factor Model
            lamb : float, l2-regularization coef for users profiles
            mu : float, l2-regularization coef for items profiles
        """
        self.n_components = n_components
        self.lamb = lamb
        self.mu = mu

    def calc_mse(self, user_ids, item_ids, true_rating):
        res = 0.
        for user_id, item_id, rating in zip(user_ids, item_ids, true_rating):
            res += (self.predict_for_pair(user_id, item_id) - rating) ** 2
        return res / user_ids.size

    def calc_mse_as_vector(self, user_ids, item_ids, true_rating):
        res = np.empty(user_ids.size)
        for i, user_id, item_id, rating in enumerate(zip(user_ids, item_ids, true_rating)):
            res[i] = (self.predict_for_pair(user_id, item_id) - rating) ** 2
        return res / user_ids.size

    def full_ridge_regression(self, X, XT):
        # fix Q, recalculate P
        QT = self.Q.T
        Q_selfadjoint = QT.dot(self.Q)
        new_diag = Q_selfadjoint.diagonal() + self.lamb
        np.fill_diagonal(Q_selfadjoint, new_diag)
        b = QT.dot(XT)
        L = cholesky(Q_selfadjoint, lower=True)
        P_new = solve_triangular(L.T, solve_triangular(L, b, lower=True), lower=False).T
        norm_p = norm(P_new - self.P)
        self.P = P_new

        # fix P, recalculate Q
        PT = self.P.T
        P_selfadjoint = PT.dot(self.P)
        new_diag = P_selfadjoint.diagonal() + self.mu
        np.fill_diagonal(P_selfadjoint, new_diag)
        b = PT.dot(X)
        L = cholesky(P_selfadjoint, lower=True)
        Q_new = solve_triangular(L.T, solve_triangular(L, b, lower=True), lower=False).T
        norm_q = norm(Q_new - self.Q)
        self.Q = Q_new
        return norm_q, norm_p

    def series_of_ridge_regressions(self, X, XT):
        # fix Q, recalculate P
        norm_p = 0.
        for user_id in range(X.shape[0]):
            user = X[user_id].toarray()[0]
            mask = user > 1e-5
            Q = self.Q[mask]
            QT = Q.T
            Q_selfadjoint = QT.dot(Q)
            new_diag = Q_selfadjoint.diagonal() + self.lamb
            np.fill_diagonal(Q_selfadjoint, new_diag)
            b = QT.dot(user[mask])
            p_u = cho_solve(cho_factor(Q_selfadjoint), b)
            norm_p += norm(p_u - self.P[user_id])
            self.P[user_id] = p_u

        # fix P, recalculate Q
        norm_q = 0.
        for item_id in range(X.shape[1]):
            item = XT[item_id].toarray()[0]
            mask = item > 1e-5
            P = self.P[mask]
            PT = P.T
            P_selfadjoint = PT.dot(P)
            new_diag = P_selfadjoint.diagonal() + self.mu
            np.fill_diagonal(P_selfadjoint, new_diag)
            b = PT.dot(item[mask])
            q_i = cho_solve(cho_factor(P_selfadjoint), b)
            norm_q += norm(q_i - self.Q[item_id])
            self.Q[item_id] = q_i
        return norm_q, norm_p

    def series_of_non_negative_ridge_regressions(self, X, XT):
        # fix Q, recalculate P
        norm_p = 0.
        for user_id in range(X.shape[0]):
            user = X[user_id].toarray()[0]
            mask = user > 1e-5
            Q = self.Q[mask]
            QT = Q.T
            Q_selfadjoint = QT.dot(Q)
            new_diag = Q_selfadjoint.diagonal() + self.lamb
            np.fill_diagonal(Q_selfadjoint, new_diag)
            b = QT.dot(user[mask])
            p_u = cho_solve(cho_factor(Q_selfadjoint), b)
            p_u[p_u < 0] = 0
            norm_p += norm(p_u - self.P[user_id])
            self.P[user_id] = p_u

        # fix P, recalculate Q
        norm_q = 0.
        for item_id in range(X.shape[1]):
            item = XT[item_id].toarray()[0]
            mask = item > 1e-5
            P = self.P[mask]
            PT = P.T
            P_selfadjoint = PT.dot(P)
            new_diag = P_selfadjoint.diagonal() + self.mu
            np.fill_diagonal(P_selfadjoint, new_diag)
            b = PT.dot(item[mask])
            q_i = cho_solve(cho_factor(P_selfadjoint), b)
            q_i[q_i < 0] = 0
            norm_q += norm(q_i - self.Q[item_id])
            self.Q[item_id] = q_i
        return norm_q, norm_p

    def fit(self, X, P_init=None, Q_init=None, validation_triplets=None, max_iter=1000, tol=1e-4, verbose=False):
        """
        Fitting of Latent Factor Model using ALS method

        Parameters:
        -----------
            X : sparse matrix, users-items matrix
            max_iter: int, maximum number of iterations
            tol: float, tolerance of the algorithm
            (if \sum_u \sum_d p_{ud}^2 + \sum_i \sum_d q_{id}^2 < tol then break)
            verbose: bool, if true then print additional information during the optimization
        """
        if P_init is not None:
            self.P = P_init
        else:
            self.P = np.random.uniform(low=0., high=np.sqrt(3 * 3.9 / self.n_components),
                                       size=(X.shape[0], self.n_components))

        if Q_init is not None:
            self.Q = Q_init
        else:
            self.Q = np.random.uniform(low=0., high=np.sqrt(3 * 3.9 / self.n_components),
                                       size=(X.shape[1], self.n_components))

        # need for faster optimization
        XT = csr_matrix(X.T)

        for iteration in range(max_iter):
            #norm_p, norm_q = self.full_ridge_regression(X, XT)
            #norm_p, norm_q = self.series_of_ridge_regressions(X, XT)
            norm_p, norm_q = self.series_of_non_negative_ridge_regressions(X, XT)

            if verbose:
                print('iter:', iteration + 1, '\tP diff norm: {0:.4f}'.format(norm_p),
                      '\tQ diff norm: {0:.4f}'.format(norm_q))
            if norm_p + norm_q <= tol:
                break


    def predict_for_pair(self, user, item):
        """
        Get the prediction

        Parameters:
        -----------
            user : non-negative int, user index

            item : non-negative int, item index
        """
        return float(self.P[user].dot(self.Q[item].T))
