import itertools
import numpy as np


class MulticlassStrategy:
    def __init__(self, classifier, mode='one_vs_all', **kwargs):
        """
        Инициализация мультиклассового классификатора

        classifier - базовый бинарный классификатор
        mode - способ решения многоклассовой задачи, либо 'one_vs_all', либо 'all_vs_all'
        **kwargs - параметры классификатор
        """
        if mode != 'one_vs_all' and mode != 'all_vs_all':
            raise TypeError("Wrong value for argument mode, should be either \
                'one_vs_all', or 'all_vs_all', but " + str(mode) + ' given')
        self.mode = mode
        self.kwargs = kwargs
        self.classifier = classifier


    def fit(self, X, y):
        """
        Обучение классификатора
        """
        self.binary_classifiers = []
        self.num_of_classes = np.max(y) + 1
        if self.mode == 'one_vs_all':
            for i in range(self.num_of_classes):
                clf = self.classifier(**self.kwargs)
                y_i = np.copy(y)
                mask = y_i == i
                # ToDo: decide what to do if no there isn't such a class
                if mask.any():
                    y_i[mask] = 1
                if (~mask).any():
                    y_i[~mask] = -1
                clf.fit(X, y_i)
                self.binary_classifiers.append(clf)
        else:
            self.combs = []
            for comb in itertools.combinations(np.arange(self.num_of_classes), 2):
                mask1 = (y == comb[0])
                mask2 = (y == comb[1])
                if mask1.any() and mask2.any():
                    clf = self.classifier(**self.kwargs)
                    mask = mask1 | mask2
                    y_tmp = np.copy(y[mask])
                    y_tmp[y_tmp == comb[0]] = -1
                    y_tmp[y_tmp == comb[1]] = 1
                    clf.fit(X[mask], y_tmp)
                    self.combs.append(comb)
                    self.binary_classifiers.append(clf)


    def predict(self, X):
        """
        Выдача предсказаний классификатором
        """
        if self.mode == 'one_vs_all':
            winners_indices = np.empty(X.shape[0])
            winners_scores = np.zeros(X.shape[0])
            for i, clf in enumerate(self.binary_classifiers):
                probas = clf.predict_proba(X)[:, 1]
                mask = probas > winners_scores
                winners_scores[mask] = probas[mask]
                winners_indices[mask] = i
            return winners_indices
        else:
            votes_of_classifiers = np.zeros((X.shape[0], self.num_of_classes))
            for comb, clf in zip(self.combs, self.binary_classifiers):
                votes = clf.predict(X)
                mask = votes == -1
                if mask.any():
                    votes_of_classifiers[mask, comb[0]] += 1
                if (~mask).any():
                    votes_of_classifiers[~mask, comb[1]] += 1
            return np.argmax(votes_of_classifiers, axis=1)