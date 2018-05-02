import oracles
import numpy as np
import time
from scipy import special, sparse
from sklearn import metrics


class PEGASOSMethod:
    """
    Реализация метода Pegasos для решения задачи svm.
    """
    def __init__(self, step_lambda=1, batch_size=100, num_iter=10000, C=1e-3):
        """
        step_lambda - величина шага, соответствует 

        batch_size - размер батча

        num_iter - число итераций метода, предлагается делать константное
        число итераций
        """
        self.step_lambda = step_lambda
        self.batch_size = batch_size
        self.num_iter = num_iter
        self.oracle = oracles.BinaryHinge(C=C)

    def prepare_mini_batch_samples(self, train_set_len):
        indices = np.arange(train_set_len)
        np.random.shuffle(indices)
        return np.hstack((indices, indices))

    def fit(self, X, y, trace=False, fit_intercept=True):
        """
        Обучение метода по выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        trace - переменная типа bool

        Если trace = True, то метод должен вернуть словарь history, содержащий информацию 
        о поведении метода. Длина словаря history = количество итераций + 1 (начальное приближение)

        history['time']: list of floats, содержит интервалы времени между двумя итерациями метода
        history['func']: list of floats, содержит значения функции на каждой итерации
        (0 для самой первой точки)
        """
        if self.batch_size > X.shape[0]:
            raise ValueError('batch size is larger than number of objects: '
                             + str(self.batch_size) + ' > ' + str(X.shape[0]))

        if fit_intercept:
            const_column = np.ones((y.size, 1))
            if sparse.issparse(X):
                X = sparse.hstack((X, sparse.csr_matrix(const_column)))
            else:
                X = np.hstack((X, const_column))

        self.w = np.zeros(X.shape[1])

        if trace:
            start_time = time.time()
            history = {'time': [0], 'func': [oracle.func(X, y, self.w)]}

        indices = self.prepare_mini_batch_samples(y.size)
        indices_cur_position = 0
        F_best = float('inf')
        for i in range(1, self.num_iter + 1):
            random_indices = indices[indices_cur_position:
                                     indices_cur_position + self.batch_size]
            indices_cur_position += self.batch_size
            if indices_cur_position >= X.shape[0]:
                indices_cur_position -= X.shape[0]

            a_i = 1 / i / self.step_lambda
            mask = y[random_indices] * X[random_indices].dot(self.w.T) < 1
            tmp = X[random_indices[mask]].T.dot(y[random_indices[mask]])
            self.w = (1 - 1 / i) * self.w + a_i / self.batch_size * tmp
            norm_sqr_w = np.sum(self.w**2)
            if not np.isclose(norm_sqr_w, 0):
                self.w = min(1, 1 / self.step_lambda / np.sqrt(norm_sqr_w)) * self.w
            F_cur = self.oracle.func(X, y, self.w)
            if F_cur < F_best:
                F_best = F_cur
                w_best = self.w
            if trace:
                history['func'].append(F_best)
                history['time'].append(time.time() - start_time)
        self.w = w_best
        if trace:
            return history

    def predict(self, X, fit_intercept=True):
        """
        Получить предсказания по выборке X

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        """
        if fit_intercept:
            return np.sign(X.dot(self.w[:-1].T) + self.w[-1])
        else:
            return np.sign(X.dot(self.w.T))