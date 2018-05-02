import numpy as np
from scipy import special, misc, sparse


class BinaryLogistic():
    """
    Оракул для задачи двухклассовой логистической регрессии.

    Оракул должен поддерживать l2 регуляризацию.
    """

    def __init__(self, l2_coef=0.):
        """
        Задание параметров оракула.

        l2_coef - коэффициент l2 регуляризации
        """
        self.l2_coef = l2_coef

    def func(self, X, y, w, bias=0.):
        """
        Вычислить значение функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array, y \in {-1, 1}
        w - одномерный numpy array
        """
        M = (X.dot(w) + bias) * y
        log_loss = np.sum(np.logaddexp(0, -M)) / X.shape[0]
        l2_regularizer = self.l2_coef / 2 * float(np.sum(w ** 2))
        return log_loss + l2_regularizer

    def grad(self, X, y, w, bias=0., calculate_for_bias=False):
        """
        Вычислить градиент функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array, y \in {-1, 1}
        w - одномерный numpy array
        """
        inverse_denom = special.expit(-y * (X.dot(w) + bias))
        if calculate_for_bias:
            loss_grad = np.sum(inverse_denom * -y)
            return loss_grad / X.shape[0]
        else:
            loss_grad = X.T.dot(inverse_denom * -y)
            return loss_grad / X.shape[0] + self.l2_coef * w


class MulticlassLogistic():
    """
    Оракул для задачи многоклассовой логистической регрессии.

    Оракул должен поддерживать l2 регуляризацию.
    w в этом случае двумерный numpy array размера (class_number, d),
    где class_number - количество классов в задаче, d - размерность задачи
    """

    def __init__(self, class_number=None, l2_coef=0.):
        """
        Задание параметров оракула.

        class_number - количество классов в задаче
        l2_coef - коэффициент l2 регуляризации
        """
        self.class_number = class_number
        self.l2_coef = l2_coef

    def func(self, X, y, w, bias=None):
        """
        Вычислить значение функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array, y = 0, 1, ...
        w - двумерный numpy array размера (class_number, d)
        """
        if self.class_number is None:
            class_number = int(np.max(y) + 1)
        else:
            class_number = int(self.class_number)

        if bias is None:
            M = X.dot(w.T)
        else:
            M = X.dot(w.T) + bias[np.newaxis, :]
        M -= np.max(M, axis=1)[:, np.newaxis]
        log_denom = misc.logsumexp(M, axis=1)
        log_nom = np.empty(y.size)
        #log_nom = M[np.arange(y.size), y]
        for j in range(class_number):
            mask = y == j
            if mask.any():
                log_nom[mask] = M[mask, j]
        loss = np.sum(log_denom - log_nom) / X.shape[0]
        return loss + self.l2_coef / 2 * float(np.sum(w ** 2))

    def softmax(self, X, w, bias=None):
        if bias is None:
            M = X.dot(w.T)
        else:
            M = X.dot(w.T) + bias[np.newaxis, :]
        M -= np.max(M, axis=1)[:, np.newaxis]
        M_exp = np.exp(M)
        softmax_denom = np.sum(M_exp, axis=1)
        return M_exp / softmax_denom[:, np.newaxis]

    def grad(self, X, y, w, bias=None, calculate_for_bias=False):
        """
        Вычислить значение функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array, y = 0, 1, ...
        w - двумерный numpy array размера (class_number, d)
        """
        if self.class_number is None:
            class_number = int(np.max(y) + 1)
        else:
            class_number = int(self.class_number)

        if bias is None:
            M = X.dot(w.T)
        else:
            M = X.dot(w.T) + bias[np.newaxis, :]
        M -= np.max(M, axis=1)[:, np.newaxis]
        M_exp = np.exp(M)
        softmax_denom = np.sum(M_exp, axis=1)

        if calculate_for_bias:
            res_grad = np.empty(class_number)
        else:
            res_grad = np.empty((class_number, w.shape[1]))

        #if calculate_for_bias:
        #    grad = (M_exp / softmax_denom[:, np.newaxis])
        #    grad[np.arange(y.size), y] -= 1
        #    res_grad = np.sum(M_exp, axis=0) / X.shape[0]
        #    return res_grad
        #else:
        #    grad = (M_exp / softmax_denom[:, np.newaxis])
        #    grad[np.arange(y.size), y] -= 1
        #    res_grad = X.T.dot(grad).T / X.shape[0]
        #    return res_grad + self.l2_coef * w

        for j in range(class_number):
            grad_on_obj = M_exp[:, j] / softmax_denom
            mask = y == j
            if mask.any():
                grad_on_obj[mask] -= 1
            if calculate_for_bias:
                res_grad[j] = np.sum(grad_on_obj) / X.shape[0]
            else:
                res_grad[j] = X.T.dot(grad_on_obj) / X.shape[0]
        if calculate_for_bias:
            return res_grad
        else:
            return res_grad + self.l2_coef * w
