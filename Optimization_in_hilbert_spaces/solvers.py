import numpy as np


class GradientDescent():
    def __init__(self, num_of_points=10000, max_iter=1000, step=1e-3, tol=1e-3, verbose=True, mode='const step'):
        self.num_of_points = num_of_points
        self.max_iter = max_iter
        self.step = step
        self.tol = tol
        self.verbose = verbose
        self.mode = mode

    def calc_deriv_by_step(self, u, step):
        deriv_1 = self.problem.calc_deriative(u - step * self.problem.calc_deriative(u))
        deriv_2 = self.problem.calc_deriative(u)
        return -self.problem.calc_dot(deriv_1, deriv_2)

    def find_best_step(self, u, tol):
        left = 0.
        right = 1.
        deriv = self.calc_deriv_by_step(u, right)
        while deriv < 0:
            right *= 2
            deriv = self.calc_deriv_by_step(u, right)
        while right - left >= tol:
            mid = (right + left) / 2
            mid_deriv = self.calc_deriv_by_step(u, mid)
            if mid_deriv > 0:
                right = mid
            elif mid_deriv < 0:
                left = mid
            else:
                return mid
        return (right + left) / 2

    def optimize(self):
        u = np.zeros(self.num_of_points)
        for iter in range(self.max_iter):
            if self.mode == 'const step':
                step = self.step
            elif self.mode == 'fastest':
                step = self.find_best_step(u, 1e-4)
            else:
                raise TypeError('Unknown mode: ' + str(mode))
            delta_u = step * self.problem.calc_deriative(u)
            u_new = u - delta_u
            delta_u = np.sqrt(delta_u.dot(delta_u))
            if self.verbose:
                print('iteration', iter + 1, '\tnorm diff =', delta_u, '\tfunctional =', self.problem.calc_functional(u))
            u = u_new
            if delta_u < self.tol:
                break
        return u


class NewtonsMethod():
    def __init__(self, num_of_points=10000, max_iter=1000, tol=1e-3, verbose=True, mode='const step'):
        self.num_of_points = num_of_points
        self.max_iter = max_iter
        self.step = step
        self.tol = tol
        self.verbose = verbose
        self.mode = mode

    def find_best_step(self, u, tol):
        left = 0.
        right = 1.
        return 1 # ToDo: finish

    def optimize(self):
        u = np.zeros(self.num_of_points)
        for iter in range(self.max_iter):
            if self.mode == 'const step':
                step = 1
            elif self.mode == 'fastest':
                step = self.find_best_step(u, 1e-4)
            else:
                raise TypeError('Unknown mode: ' + str(mode))
            delta_u = step * cho_solve(cho_factor(self.problem.calc_hessian(u)), self.problem.calc_deriative(u))
            u_new = u - delta_u
            delta_u = np.sqrt(delta_u.dot(delta_u))
            if self.verbose:
                print('iteration', iter + 1, '\tnorm diff =', delta_u, '\tfunctional =', self.problem.calc_functional(u))
            u = u_new
            if delta_u < self.tol:
                break
        return u


class SimplexMethod():
    def __init__(self):
        pass

    def optimize(self):
        ratings = -self.problem.c.astype('float')
        A = self.problem.A.astype('float')
        b = self.problem.b.astype('float')
        variables_permutation = np.empty(A.shape[1])
        # Assumption: there is already identity submatrix in last m columns
        variables_permutation = np.arange(A.shape[1])[-A.shape[0]:]

        decisive_col = np.argmin(ratings)
        while ratings[decisive_col] < -1e-6:
            print ('iter')
            # ToDo: think about devision by 0
            # change b vector
            b = b / A[:, decisive_col]
            values = b.copy()
            # find minimal positive value in b
            values[values <= 0] = float('inf')
            decisive_row = np.argmin(values)
            # Add variable in permutation
            variables_permutation[decisive_row] = decisive_col
            # save previous state of decisive column
            prev_col_values = A[:, decisive_col].copy()
            # make the decisive col unit
            A = A / prev_col_values[:, np.newaxis]
            # mask of nondecisive rows
            nondecisive_rows = np.zeros(self.problem.A.shape[0], dtype='bool')
            nondecisive_rows[decisive_row] = True
            nondecisive_rows = ~nondecisive_rows
            # make from decisive column a basis vector
            A[nondecisive_rows, :] -= A[decisive_row][np.newaxis, :]
            b[nondecisive_rows] -= b[decisive_row]
            # return previous values of nondecisive rows and support vector
            A[nondecisive_rows, :] *= prev_col_values[nondecisive_rows, np.newaxis]
            b[nondecisive_rows] *= prev_col_values[nondecisive_rows]
            # refresh vector of ratings
            ratings = ratings - A[decisive_row] * ratings[decisive_col]
            # find new decisive column
            decisive_col = np.argmin(ratings)
        res = np.zeros_like(ratings)
        res[variables_permutation[variables_permutation != -1]] = b[(variables_permutation != -1)[:b.size]]
        return res