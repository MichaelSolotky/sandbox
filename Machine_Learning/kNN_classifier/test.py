from unittest import TestCase
from nearest_neighbors import KNNClassifier
import cross_validation
from sklearn.model_selection import cross_val_score
from scipy.sparse import *
from sklearn.metrics.pairwise import cosine_distances, euclidean_distances
import numpy as np
from sklearn.metrics import accuracy_score
from sklearn.metrics import fbeta_score, make_scorer
from sklearn.neighbors import KNeighborsClassifier


class TestKNNClassifier(TestCase):
    def test_metric_computation(self):
        knn = KNNClassifier(metric='cosine')
        X_train = np.array([[0, 1], [1, 1], [2, 3]])
        X_test = np.array([[4, 1], [4, 3]])
        knn.fit(X_train)
        dist_matrix = knn.pairwise_distance(X_test)
        assert (dist_matrix == cosine_distances(X_test, X_train)).all()

        knn = KNNClassifier(metric='euclidean')
        X_train = np.array([[0, 1e-5], [1, 1], [2, 3]])
        X_test = np.array([[4, 1], [4, 3], [-6, 8]])
        knn.fit(X_train)
        dist_matrix = knn.pairwise_distance(X_test)
        assert (dist_matrix == euclidean_distances(X_test, X_train)).all()

    def test_matrix_types(self):
        knn = KNNClassifier(metric='cosine', k=2)
        X = csr_matrix([[1, 2], [2, 3], [4, 5]])
        y = np.array([1, 2, 3])
        knn.fit(X=X, y=y)
        X_test = csr_matrix([[4, 1], [4, 3], [-6, 8]])
        dist = knn.pairwise_distance(X_test)
        assert (dist == cosine_distances(X_test, X)).all()
        assert (knn.predict(X) == y).all()

        knn = KNNClassifier(metric='euclidean', k=2)
        X = csr_matrix([[0, 1e-5], [2, 3], [4, 5]])
        y = np.array([1, 2, 3])
        knn.fit(X=X, y=y)
        X_test = csr_matrix([[4, 1], [4, 3], [-6, 8]])
        assert (knn.pairwise_distance(X_test) == euclidean_distances(X_test, X)).all()
        assert (knn.predict(X) == y).all()

    def test_k_range(self):
        for i in range(1):
            knn = KNNClassifier(metric='euclidean', k=i + 1)
            X = csr_matrix([[1], [2], [3], [4], [5], [6], [7], [8], [9], [10]])
            y = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
            knn.fit(X, y)
            X_test = csr_matrix([[1], [2], [3], [4], [5], [6], [7], [8], [9], [10]])
            assert (knn.pairwise_distance(X_test) == euclidean_distances(X_test, X)).all()
            assert (knn.predict(X) == y).all()

    def test_weights(self):
        knn = KNNClassifier(metric='euclidean', k=2)
        X = csr_matrix([[1, 1], [2, 2], [1.5, 1.5]])
        y = np.array([1, 2, 1])
        knn.fit(X=X, y=y)
        X_test = csr_matrix([[1.9, 1.9]])
        assert (knn.predict(X_test) == 2).all()

    def test_kfold(self):
        res = cross_validation.kfold(10, 3)
        assert len(res) == 3
        for i in range(3):
            assert len(set(res[i][0]) & set(res[i][1])) == 0
            assert set(res[i][0]) | set(res[i][1]) == set(np.arange(10))

        res = cross_validation.kfold(10000, 4)
        assert len(res) == 4
        for i in range(4):
            assert len(set(res[i][0]) & set(res[i][1])) == 0
            assert set(res[i][0]) | set(res[i][1]) == set(np.arange(10000))

        res = cross_validation.kfold(5, 1)
        assert len(res) == 1
        for i in range(1):
            assert len(set(res[i][0]) & set(res[i][1])) == 0
            assert set(res[i][0]) | set(res[i][1]) == set(np.arange(5))

    def test_accuracy(self):
        y_real = np.array([1, 2, 1, 2, 1, 1, 1, 2])
        y_predicted = np.array([1, 2, 3, 1, 1, 2, 3, 1])
        my_acc = cross_validation.calculate_score(y_real, y_predicted, score='accuracy')
        assert my_acc == accuracy_score(y_real, y_predicted)

    def test_cross_val_score(self):
        knn = KNNClassifier(metric='euclidean', k=2)
        X = csr_matrix([[0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5],
                        [0, 1e-5], [2, 3], [4, 5]])
        y = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                      1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
        knn.fit(X=X, y=y)
        cv = cross_validation.kfold(30, 3)
        my_res = cross_validation.knn_cross_val_score(X, y, k_list=[1], cv=cv)
        sklearn_res = cross_val_score(KNeighborsClassifier(), X, y, cv=cv,
                                      scoring=make_scorer(accuracy_score))
        assert (my_res[1] == sklearn_res).all()


if __name__ == 'main':
    unittest.main()
