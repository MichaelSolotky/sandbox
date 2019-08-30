import oracles
import numpy as np
import time
from scipy import special, spatial, sparse
from sklearn import metrics
from sklearn.base import BaseEstimator
import cvxopt


class SVMSolver(BaseEstimator):
    """
    Класс с реализацией SVM через метод внутренней точки.
    """
    def __init__(self, C, method, kernel='linear', gamma=0.5, degree=3):
        """
        C - float, коэффициент регуляризации

        method - строка, задающая решаемую задачу, может принимать значения:
            'primal' - соответствует прямой задаче
            'dual' - соответствует двойственной задаче
        kernel - строка, задающая ядро при решении двойственной задачи
            'linear' - линейное
            'polynomial' - полиномиальное
            'rbf' - rbf-ядро
        Обратите внимание, что часть функций класса используется при одном методе решения,
        а часть при другом
        """
        if method != 'primal' and method != 'dual':
            raise ValueError('unknown method: ' + str(method))
        self.method = method
        #if kernel != 'linear' and kernel != 'polynomial' and kernel != 'rbf':
        #    raise ValueError('unknown kernel: ' + str(kernel))
        if kernel != 'linear' and method != 'dual':
            raise TypeError('Only dual method can adjust kernel-trick')
        self.kernel = kernel
        self.C = C
        self.gamma = gamma
        self.degree = degree

    def rbf(self, X_1, X_2):
        dist = spatial.distance.cdist(X_1, X_2, metric='euclidean') ** 2
        return np.exp(-self.gamma * dist)

    def decision_function(self, X):
        if self.kernel == 'linear':
            return X.dot(self.w) + self.w0
        elif self.kernel == 'rbf':
            rbf_matrix = self.rbf(X, self.support_vectors)
            return rbf_matrix.dot(self.lambdas[self.lambdas > 0] * self.support_targets)
        elif self.kernel == 'polynomial':
            gramm_matrix = (X.dot(self.support_vectors.T) + 1) ** self.degree
            return gramm_matrix.dot(self.lambdas[self.lambdas > 0] * self.support_targets)

    def compute_primal_objective(self, X, y):
        """
        Метод для подсчета целевой функции SVM для прямой задачи

        X - переменная типа numpy.array, признаковые описания объектов из обучающей выборки
        y - переменная типа numpy.array, правильные ответы на обучающей выборке,
        """
        if self.method != 'primal':
            raise ValueError('attempt to compute primal objective while method is dual')
        l = X.shape[0]
        ksi = 1 - y * (X.dot(self.w) + self.w0)
        ksi[ksi < 0] = 0
        return 0.5 * np.sum(self.w ** 2) + self.C / l * np.sum(ksi)

    def compute_dual_objective(self, X, y):
        """
        Метод для подсчёта целевой функции SVM для двойственной задачи

        X - переменная типа numpy.array, признаковые описания объектов из обучающей выборки
        y - переменная типа numpy.array, правильные ответы на обучающей выборке,
        """
        if self.method != 'dual':
            raise ValueError('attempt to compute dual objective while method is primal')
        tmp_var = y * self.lambdas
        Q =  tmp_var[np.newaxis, :] * tmp_var[:, np.newaxis]
        Q = Q.astype(float)
        if self.kernel == 'linear':
            gramm_matrix = X.dot(X.T)
        elif self.kernel == 'polynomial':
            gramm_matrix = (X.dot(X.T) + 1) ** self.degree
        elif self.kernel == 'rbf':
            dist = spatial.distance.cdist(X, X, metric='euclidean') ** 2
            gramm_matrix = np.exp(-self.gamma * dist)
        Q *= gramm_matrix # 1/2 sum_{i = 1}^l sum_{j = 1}^l \lambda_i lambda_j y_i y_j
        return 0.5 * np.sum(Q) - np.sum(self.lambdas)

    def fit(self, X, y, tolerance=1e-5, max_iter=10000):
        """
        Метод для обучения svm согласно выбранной в method задаче

        X - переменная типа numpy.array, признаковые описания объектов из обучающей выборки
        y - переменная типа numpy.array, правильные ответы на обучающей выборке,
        tolerance - требуемая точность для метода обучения
        max_iter - максимальное число итераций в методе

        """
        l, d = X.shape
        cvxopt.solvers.options['abstol'] = tolerance
        #cvxopt.solvers.options['reltol'] = tolerance
        cvxopt.solvers.options['max_iters'] = max_iter

        if self.method == 'primal':
            diag = np.zeros(l + d + 1)
            diag[:d] = 1
            Q = np.diag(diag) # <w, w>
            Q = cvxopt.matrix(Q)
            p = np.zeros(l + d + 1)
            p[d + 1:] = self.C / l # C * /sum_{i = 1}^l \ksi_i
            p = cvxopt.matrix(p)
            G = np.zeros((2 * l, l + d + 1))
            np.fill_diagonal(G[:l, d + 1:], -1)
            np.fill_diagonal(G[l:, d + 1:], -1)
            G[:l, :d] = -X * y[:, np.newaxis]
            G[:l, d] = -y
            G = cvxopt.matrix(G)
            h = np.zeros(2 * l)
            h[:l] = -1
            h = cvxopt.matrix(h)
            solution = np.array(cvxopt.solvers.qp(Q, p, G, h)['x']).ravel()
            self.w = solution[:d]
            self.w0 = solution[d]
        elif self.method == 'dual':
            Q = y[:, np.newaxis] * y[np.newaxis, :]
            Q = Q.astype(float)
            if self.kernel == 'linear':
                gramm_matrix = X.dot(X.T)
            elif self.kernel == 'polynomial':
                gramm_matrix = (X.dot(X.T) + 1) ** self.degree
            elif self.kernel == 'rbf':
                dist = spatial.distance.cdist(X, X, metric='euclidean') ** 2
                gramm_matrix = np.exp(-self.gamma * dist)
            Q *= gramm_matrix # 1/2 sum_{i = 1}^l sum_{j = 1}^l \lambda_i lambda_j y_i y_j
            Q = cvxopt.matrix(Q)
            p = cvxopt.matrix(-np.ones(l, dtype=float)) # -\sum_lambda_i
            G = np.zeros((2 * l, l), dtype=float)
            np.fill_diagonal(G[:l], -1)
            np.fill_diagonal(G[l:], 1)
            G = cvxopt.matrix(G)
            h = np.empty(2 * l, dtype=float)
            h[:l].fill(0)
            h[l:].fill(self.C / l)
            h = cvxopt.matrix(h)
            A = cvxopt.matrix(np.array(y[np.newaxis, :], dtype=float))
            b = cvxopt.matrix(np.array([0], dtype=float))
            self.lambdas = np.array(cvxopt.solvers.qp(Q, p, G, h, A, b)['x']).ravel()
            mask = self.lambdas > 0 # ToDo: dicide how far to step from zero
            self.support_vectors = np.copy(X[mask])
            self.support_targets = np.copy(y[mask])

            if self.kernel == 'linear':
                self.w = self.get_w(X, y)
                self.w0 = self.get_w0(X, y)

    def predict(self, X):
        """
        Метод для получения предсказаний на данных

        X - переменная типа numpy.array, признаковые описания объектов из обучающей выборки
        """
        return np.sign(self.decision_function(X))

    def get_w(self, X=None, y=None):
        """
        Получить прямые переменные (без учёта w0)

        Если method = 'dual', а ядро линейное, переменные должны быть получены
        с помощью выборки (X, y)

        return: одномерный numpy array
        """
        if self.method == 'dual':
            if self.kernel == 'linear':
                if X is not None and y is not None:
                    return np.sum(X * (y * self.lambdas)[:, np.newaxis], axis=0)
                else:
                    return np.sum(self.support_vectors * (self.support_targets * self.lambdas)[:, np.newaxis], axis=0)
            else:
                raise ValueError('Can\'t get w in case of nonlinear kernel')
        return self.w

    def get_w0(self, X=None, y=None):
        """
        Получить вектор сдвига

        Если method = 'dual', а ядро линейное, переменные должны быть получены
        с помощью выборки (X, y) 

        return: float
        """
        if self.method == 'dual':
            if self.kernel == 'linear':
                w = self.get_w(X, y)
                if X is not None and y is not None:
                    marginal_objects = (self.lambdas > 0) & (self.lambdas < self.C / self.lambdas.size)
                    if marginal_objects is not None:
                        return X[marginal_objects][0].T.dot(w) - y[marginal_objects][0]
                    else:
                        return np.mean(X.dot(w) - y)
                else:
                    marginal_objects = (self.lambdas > 0) & (self.lambdas < self.C / self.lambdas.size)
                    if marginal_objects is not None:
                        return self.support_vectors[marginal_objects][0].T.dot(w) - self.support_targets[marginal_objects][0]
                    else:
                        return np.mean(self.support_vectors.dot(w) - self.support_targets)
            else:
                raise ValueError('Can\'t get w in case of nonlinear kernel')

        return self.w0

    def get_dual(self):
        """
        Получить двойственные переменные

        return: одномерный numpy array
        """
        if self.method != 'dual':
            raise ValueError('Can\'t get dual variables while method is primal')
        return self.lambdas
