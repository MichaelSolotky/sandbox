from unittest import TestCase
from collections import Iterable, OrderedDict
import oracles
import optimization
import utils
import multiclass
import numpy as np
from scipy import special, misc, sparse


class TestBinary(TestCase):
    def test_func(self):
        oracle = oracles.BinaryLogistic()
        X = np.array([[1, 0], [0, 1]])
        y = np.array([-1, 1])
        w = np.array([1, 1])
        res = oracle.func(X, y, w)
        correct = float(np.mean(np.log(1 + np.exp(-y * np.dot(X, w)))))
        assert res == correct

    def test_func_sparse_crs(self):
        oracle = oracles.BinaryLogistic()
        X_init = np.random.random((10000, 2))
        X = sparse.csr_matrix(X_init)
        y = np.random.randint(0, 2, 10000) * 2 - 1
        w = np.array([1, 1])
        res = oracle.func(X, y, w)
        correct = float(np.mean(np.log(1 + np.exp(-y * np.dot(X_init, w)))))
        assert np.isclose(res, correct)

    def test_func_with_bias(self):
        oracle = oracles.BinaryLogistic()
        X = np.array([[1, 0], [0, 1]])
        y = np.array([-1, 1])
        w = np.array([1, 1])
        bias = 0.5
        res = oracle.func(X, y, w, bias=bias)
        correct = float(np.mean(np.log(1 + np.exp(-y * (np.dot(X, w) + bias)))))
        assert res == correct

    def test_func_regularizer(self):
        l2 = np.random.normal()
        oracle = oracles.BinaryLogistic(l2)
        X = np.array([[1, 0], [0, 1]])
        y = np.array([-1, 1])
        w = np.array([1, 1])
        res = oracle.func(X, y, w)
        correct = float(np.mean(np.log(1 + np.exp(-y * np.dot(X, w)))) + l2 / 2 * np.sum(w ** 2))
        assert res == correct

    def test_func_regularizer_with_bias(self):
        l2 = np.random.normal()
        oracle = oracles.BinaryLogistic(l2_coef=l2)
        X = np.array([[1, 0], [0, 1]])
        y = np.array([-1, 1])
        w = np.array([1, 1])
        bias = 0.5
        res = oracle.func(X, y, w, bias=bias)
        correct = float(np.mean(np.log(1 + np.exp(-y * (np.dot(X, w) + bias)))) + l2 / 2 * np.sum(w ** 2))
        assert res == correct

    def test_grad(self):
        oracle = oracles.BinaryLogistic()
        X = np.array([[1, 0], [0, 1]])
        y = np.array([-1, 1])
        w = np.array([1, 1])
        res = oracle.grad(X, y, w)
        correct = -1 / X.shape[0] * np.dot((X * y[:, np.newaxis]).T, 1 / \
        (1 + np.exp(y * np.dot(X, w))))
        assert (res == correct).all()

    def test_grad_csr(self):
        oracle = oracles.BinaryLogistic()
        X_init = np.random.random((10000, 2))
        X = sparse.csr_matrix(X_init)
        y = np.random.randint(0, 2, 10000) * 2 - 1
        w = np.array([1, 1])
        res = oracle.grad(X, y, w)
        correct = -1 / X.shape[0] * np.dot((X.toarray() * y[:, np.newaxis]).T, 1 / \
            (1 + np.exp(y * np.dot(X.toarray(), w))))
        assert np.isclose(res, correct).all()

    def test_grad_stability(self):
        np.random.seed(1693)
        l2_coef = np.random.randint(0, 10)
        l, d = 1000, 10
        my_oracle = oracles.BinaryLogistic(l2_coef=l2_coef)
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        w = np.random.random(d)
        res_grad = my_oracle.grad(X, y, w).ravel()
        res = " ".join([str(x) for x in res_grad])
        correct = '0.202871756284 0.198790762401 0.209722681265 0.202663468679 ' + \
        '0.19780804969 0.197632102817 0.200411120387 0.200786868065 0.203668088146 ' + \
        '0.193438319026'

class TestMultinomial(TestCase):
    def test_func(self):
        oracle = oracles.MulticlassLogistic()
        X = np.array([[1, 0], [0, 1]])
        y = np.array([0, 1])
        w = np.array([[1, 1], [-1, 0]])
        res = oracle.func(X, y, w)
        correct = np.log(np.exp(np.dot(X[0], w[0])) / np.sum(np.exp(np.dot(X[0], w.T))))
        correct += np.log(np.exp(np.dot(X[1], w[1])) / np.sum(np.exp(np.dot(X[1], w.T))))
        correct = -correct / 2
        assert np.isclose(res, correct)

    def test_func_regularizer(self):
        my_oracle = oracles.MulticlassLogistic(l2_coef=1)
        res = my_oracle.func(np.zeros((6, 6)), np.array([1, 2, 2, 1, 1, 2]), np.ones((3, 6)))
        correct = 10.0986122887
        assert np.isclose(res, correct)

    def test_grad_stability(self):
        np.random.seed(7998)
        l2_coef = np.random.randint(0, 10)
        l, d = 1000, 10
        my_oracle = oracles.MulticlassLogistic(l2_coef=l2_coef)
        X = np.random.random((l, d))
        y = np.random.randint(0, 3, l)
        w = np.random.random((3, d))
        res_grad = my_oracle.grad(X, y, w).ravel()
        res = " ".join([str(x) for x in res_grad])
        correct = '3.13676597791 0.900698078761 -0.021975681637 4.907951258 ' + \
                  '5.23707049038 0.168585188245 4.25469195735 2.93336951315 ' + \
                  '3.62071946381 5.6927975983 2.93019021237 2.02178153878 ' + \
                  '0.618947485041 5.0800252781 6.54707234327 0.613098582567 ' + \
                  '1.91655708654 4.81614449 3.24035619107 1.83861747206 ' + \
                  '4.53177823263 6.04887146181 4.02833279661 4.86217967278 ' + \
                  '3.92232965231 4.26420687572 2.85928390281 4.90074402382 ' + \
                  '1.29484178027 2.01087204272'
        assert res == correct
