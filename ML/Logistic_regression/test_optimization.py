from unittest import TestCase
from collections import Iterable, OrderedDict
import oracles
import optimization
import utils
import multiclass
import numpy as np
import time
from scipy import special, misc


class TestGD(TestCase):
    # Test wall time and functionality (that it doesn't crash)
    def test_fit_binary(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        clf = optimization.GDClassifier('binary_logistic', max_iter=1000)
        clf.fit(X, y)
        assert time.time() - start_time < 2

    def test_fit_multinomial(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        clf = optimization.GDClassifier('multinomial_logistic', max_iter=100)
        clf.fit(X, y)
        assert time.time() - start_time < 2

    def test_predict_proba_binary(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        clf = optimization.GDClassifier('binary_logistic', max_iter=1000)
        clf.fit(X, y)
        clf.predict_proba(X)
        assert time.time() - start_time < 2

    def test_predict_proba_multinomial(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        clf = optimization.GDClassifier('multinomial_logistic', max_iter=100)
        clf.fit(X, y)
        clf.predict_proba(X)
        assert time.time() - start_time < 2

    def test_history(self):
        np.random.seed(10)
        clf = optimization.GDClassifier(loss_function='binary_logistic', step_alpha=1,
            step_beta=0, tolerance=1e-4, max_iter=5, l2_coef=0.1)
        l, d = 1000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        w = np.random.random(d)
        history = clf.fit(X, y, w_0=np.zeros(d), trace=True)
        res = ' '.join([str(x) for x in history['func']])
        correct = '0.69314718056 0.692686844381 0.692523579699 0.692412212011 ' + \
                  '0.692327505075'
        assert correct == res

    def test_early_stop(self):
        np.random.seed(11)
        clf = optimization.GDClassifier(loss_function='binary_logistic', step_alpha=1,
            step_beta=0, tolerance=1e-4, max_iter=5, l2_coef=0.1)
        l, d = 1000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        w = np.random.random(d)
        history = clf.fit(X, y, w_0=np.zeros(d), trace=True)
        res = ' '.join([str(x) for x in history['func']])
        correct = '0.69314718056 0.692490985601 0.69233649967 0.692240987231'
        assert res == correct

    def test_stability(self):
        np.random.seed(19)
        clf = optimization.GDClassifier(loss_function='binary_logistic', step_alpha=1,
            step_beta=0, tolerance=1e-4, max_iter=5, l2_coef=0.1)
        l, d = 1000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        w = np.random.random(d)
        history = clf.fit(X, y, w_0=np.zeros(d), trace=True)
        res = ' '.join([str(x) for x in history['func']])
        correct = '0.69314718056 0.692787148596 0.692572983989 0.692411128922 ' + \
                  '0.692285836002 0.692188643297'
        assert res == correct

class TestSGD(TestCase):
    def test_fit_binary(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        clf = optimization.SGDClassifier('binary_logistic', max_iter=10000)
        clf.fit(X, y)
        assert time.time() - start_time < 2

    def test_predict_proba_binary(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(0, 2, l) * 2 - 1
        clf = optimization.SGDClassifier('binary_logistic', max_iter=10000)
        clf.fit(X, y)
        clf.predict_proba(X)
        assert time.time() - start_time < 12

    def test_fit_multinomial(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(1, 5, l)
        clf = optimization.SGDClassifier('multinomial_logistic',
                                         class_number=int(np.max(y) + 1),
                                         max_iter=1000)
        clf.fit(X, y)
        assert time.time() - start_time < 12

    def test_predict_proba_multinomial(self):
        start_time = time.time()
        l, d = 10000, 10
        X = np.random.random((l, d))
        y = np.random.randint(1, 5, l)
        clf = optimization.SGDClassifier('multinomial_logistic', max_iter=1000,
                                         class_number=int(np.max(y) + 1))
        clf.fit(X, y)
        clf.predict_proba(X)
        assert time.time() - start_time < 12
