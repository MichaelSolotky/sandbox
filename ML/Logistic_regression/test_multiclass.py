from unittest import TestCase
from collections import Iterable, OrderedDict
import oracles
import optimization
import utils
import multiclass
import numpy as np
from scipy import special, misc
from sklearn import linear_model, svm


class TestMulticlass(TestCase):
    def test_one_vs_all(self):
        my_clf = multiclass.MulticlassStrategy(linear_model.LogisticRegression,
                                               mode='one_vs_all')
        sklearn_clf = linear_model.LogisticRegression()
        X = np.random.random((1000, 10))
        y = np.random.randint(0, 10, 1000)
        my_clf.fit(X, y)
        sklearn_clf.fit(X, y)
        my_y_pred = my_clf.predict(X)
        sklearn_y_pred = sklearn_clf.predict(X)
        acc = np.mean(my_y_pred == sklearn_y_pred)
        assert acc == 1

    def test_all_vs_all(self):
        my_clf = multiclass.MulticlassStrategy(svm.SVC, mode='all_vs_all')
        sklearn_clf = svm.SVC()
        X = np.random.random((1000, 10))
        y = np.random.randint(0, 10, 1000)
        my_clf.fit(X, y)
        sklearn_clf.fit(X, y)
        my_y_pred = my_clf.predict(X)
        sklearn_y_pred = sklearn_clf.predict(X)
        acc = np.mean(my_y_pred == sklearn_y_pred)
        assert acc == 1

    def test_binary_one_vs_all(self):
        my_clf = multiclass.MulticlassStrategy(linear_model.LogisticRegression,
                                               mode='one_vs_all')
        sklearn_clf = linear_model.LogisticRegression()
        X = np.random.random((1000, 10))
        y = np.random.randint(0, 10, 1000)
        my_clf.fit(X, y)
        for bin_clf in my_clf.binary_classifiers:
            assert bin_clf.coef_.shape[0] == 1

    def test_binary_all_vs_all(self):
        my_clf = multiclass.MulticlassStrategy(svm.SVC, mode='all_vs_all')
        sklearn_clf = svm.SVC()
        X = np.random.random((1000, 10))
        y = np.random.randint(0, 10, 1000)
        my_clf.fit(X, y)
        for bin_clf in my_clf.binary_classifiers:
            assert bin_clf.dual_coef_.shape[0] == 1
