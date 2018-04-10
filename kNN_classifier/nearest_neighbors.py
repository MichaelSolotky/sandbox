from sklearn.neighbors import NearestNeighbors
import numpy as np
import multiprocessing
from scipy.sparse import *


class KNNClassifier:
    def __init__(self, k=4, strategy='my_own', metric='cosine',
                 weights=True, test_block_size=1000):
        self.k = k
        self.strategy = strategy
        self.metric = metric  # str instance
        self.weights = weights  # bool instance
        self.test_block_size = test_block_size
        if strategy != 'my_own':
            self.sklearn_implementation = NearestNeighbors(n_neighbors=k,
                                                           algorithm=strategy,
                                                           metric=metric,
                                                           n_jobs=-1)

    def fit(self, X, y=None):
        if self.strategy == 'my_own':
            if issparse(X):
                self.X = X.toarray().astype(float)
            elif type(X).__module__ == np.__name__:
                self.X = X.astype(float)
            else:
                raise TypeError('Wrong type of testing set matrix' + str(type(X)))
        else:
            self.sklearn_implementation.fit(X, y)
        if y is None:
            self.y = None
        else:
            self.y = np.array(y).astype(int)
        if y is None:
            self.num_of_classes = None
        else:
            self.num_of_classes = np.amax(self.y) + 1

    def pairwise_distance(self, X):
        """Calculate distances between objects from X and the training set (self.X)
        Return a matrix of shape (X.shape[0], self.X.shape[0])
        """

        # dot_product calculation takes the most memory, that's why it's the first
        if issparse(X):
            X = X.toarray().astype(float)
        elif type(X).__module__ == np.__name__:
            X = X.astype(float)
        else:
            raise TypeError('Wrong type of testing set matrix') 
        if self.metric == 'euclidean':
            dot_product = 2 * np.dot(X, np.transpose(self.X))
            X_squares = np.sum(X ** 2, axis=1)
            X_train_squares = np.sum(self.X ** 2, axis=1)
            sum_squares = X_squares[:, np.newaxis] + X_train_squares[np.newaxis, :]
            return np.sqrt(sum_squares - dot_product)
        elif self.metric == 'cosine':
            X_norms = np.sqrt(np.sum(X ** 2, axis=1))[:, np.newaxis]
            if (X_norms == 0).any():
                raise TypeError('Found 0 vector in testing set -> cannot calculate cosine_metric')
            X_train_norms = np.sqrt(np.sum(self.X ** 2, axis=1))[:, np.newaxis]
            if (X_train_norms == 0).any():
                raise TypeError('Found 0 vector in training set -> cannot calculate cosine_metric')
            cosine_sim = np.dot(X / X_norms, np.transpose(self.X / X_train_norms))
            cosine_dist = 1 - cosine_sim
            return cosine_dist
        else:
            raise TypeError('metric ' + str(self.metric) + ' is not supported')

    def mini_batch_dist_computation(self, num_of_partitions, nearest_neigh_pos, X, return_distance, dist):
        # ToDo: write multithreading (with num of cpu)
        for i in range(num_of_partitions):
            begin = i * self.test_block_size
            end = (i + 1) * self.test_block_size
            if self.strategy == 'my_own':
                dist_batch = self.pairwise_distance(X[begin:end])
                # matrix, rows of which are positions of k nearest neighbors
                nearest_neigh_pos[begin:end] = np.argpartition(
                        dist_batch, np.arange(self.k), axis=1)[:, :self.k]
                if return_distance:
                    indices = np.where((nearest_neigh_pos == nearest_neigh_pos)[begin:end])
                    indices = (indices[0], nearest_neigh_pos[begin:end].ravel())
                    dist[begin:end] = dist_batch[indices].reshape(-1, self.k)
            else:
                if return_distance:
                    tmp = self.sklearn_implementation.kneighbors(X[begin:end], return_distance=True)
                    dist[begin:end] = tmp[0]
                    nearest_neigh_pos[begin:end] = tmp[1]
                else:
                    nearest_neigh_pos[begin:end] = self.sklearn_implementation.kneighbors(X[begin:end],
                                                                                          return_distance=False)

    def find_kneighbors(self, X, return_distance=True):
        # if return_distance == True: returns a tuple (distances, indices)
        # else just return indices
        if issparse(X):
            X = X.toarray().astype(float)
        elif type(X).__module__ == np.__name__:
            X = X.astype(float)
        else:
            raise TypeError('Wrong type of testing set matrix: ' + str(type(X)))
        num_of_partitions = X.shape[0] // self.test_block_size
        num_of_partitions += 1 if (X.shape[0] % self.test_block_size != 0) else 0
        nearest_neigh_pos = np.empty((X.shape[0], self.k), dtype=int)
        if return_distance:
            dist = np.empty((X.shape[0], self.k))
        else:
            dist = None
        self.mini_batch_dist_computation(num_of_partitions, nearest_neigh_pos, X, return_distance, dist)
        if return_distance:
            return (dist, nearest_neigh_pos)
        else:
            return nearest_neigh_pos

    def predict(self, X):
        if issparse(X):
            X = X.toarray().astype(float)
        elif type(X).__module__ == np.__name__:
            X = X.astype(float)
        else:
            raise TypeError('Wrong type of testing set matrix')
        if self.weights:
            kneighbors = self.find_kneighbors(X, return_distance=True)
            dist = kneighbors[0]
            eps = 1e-5
            weights = 1 / (dist + eps)
            classes_of_neigh = self.y[kneighbors[1]]
            frequencies_of_classes = np.apply_along_axis(func1d=np.bincount,
                                                         axis=1,
                                                         arr=classes_of_neigh,
                                                         minlength=self.num_of_classes)
            wieghts_of_neigh = np.empty((X.shape[0], self.num_of_classes), order='F')

            for i in range(self.num_of_classes):
                mask_of_a_class = classes_of_neigh == i
                wieghts_of_neigh[:, i] = np.sum(weights * mask_of_a_class, axis=1)
            y = np.argmax(wieghts_of_neigh, axis=1)
        else:
            kneighbors = self.find_kneighbors(X, return_distance=False)
            classes_of_neigh = self.y[kneighbors]
            frequencies_of_classes = np.apply_along_axis(func1d=np.bincount,
                                                         axis=1,
                                                         arr=classes_of_neigh,
                                                         minlength=self.num_of_classes)
            y = np.argmax(frequencies_of_classes, axis=1)
        return y
