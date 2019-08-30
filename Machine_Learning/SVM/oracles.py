import numpy as np
from scipy import special, misc, sparse


class BinaryHinge():
    """
    Оракул для задачи двухклассового линейного SVM.

    Нулевая координата вектора w соответствует w_0.
    Считается, что в классификатор подаётся X с уже созданным единичным столбцом
    (так делается для того, чтобы не переписывать код из предыдущего задания).
    """

    def __init__(self, C=1000):
        """
        Задание параметров оракула.
        """
        self.C = C

    def func(self, X, y, w):
        """
        Вычислить значение функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w - одномерный numpy array
        """
        losses = 1 - y * X.dot(w)
        losses[losses < 0] = 0
        return losses.sum() / X.shape[0] * self.C + 0.5 * np.sum(w[1:]**2)

    def grad(self, X, y, w):
        """
        Вычислить субградиент функционала в точке w на выборке X с ответами y.
        Субгрдиент в точке 0 необходимо зафиксировать равным 0.

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w - одномерный numpy array
        """
        mask = y * X.dot(w) < 1
        grads = np.zeros(X.shape)
        if mask.any():
            grads[mask] = -X[mask] * y[mask][:, np.newaxis]
        w[0] = 0
        return np.sum(grads, axis=0) / X.shape[0] * self.C + w