import numpy as np
from scipy.stats import multinomial
from nearest_neighbors import KNNClassifier


def kfold(n, n_folds=3):
    multinomial_sample = np.random.choice(n_folds, n)
    indices = np.arange(n)
    res = []
    for i in range(n_folds):
        mask = multinomial_sample == i
        res.append((indices[~mask], indices[mask]))
    return res


def calculate_score(y_real, y_predicted, score='accuracy'):
    if score == 'accuracy':
        return np.sum(y_real == y_predicted) / y_real.size
    elif not isinstance(score, str):
        raise TypeError('argument score should be str, but it\'s ' + str(type(score)))
    else:
        raise TypeError('score ' + score + ' is not supported')


def cross_val_predict(knn_classifier, num_of_classes, kneighbors):
    if isinstance(kneighbors, tuple):
        dist = kneighbors[0]
        eps = 1e-5
        weights = 1 / (dist + eps)
        classes_of_neigh = knn_classifier.y[kneighbors[1]]
        frequencies_of_classes = np.apply_along_axis(func1d=np.bincount,
                                                     axis=1,
                                                     arr=classes_of_neigh,
                                                     minlength=num_of_classes)
        wieghts_of_neigh = np.empty((kneighbors[0].shape[0], num_of_classes), order='F')

        for i in range(num_of_classes):
            mask_of_a_class = classes_of_neigh == i
            wieghts_of_neigh[:, i] = np.sum(weights * mask_of_a_class, axis=1)
        y = np.argmax(wieghts_of_neigh, axis=1)
    else:
        classes_of_neigh = knn_classifier.y[kneighbors]
        frequencies_of_classes = np.apply_along_axis(func1d=np.bincount,
                                                     axis=1,
                                                     arr=classes_of_neigh,
                                                     minlength=num_of_classes)
        y = np.argmax(frequencies_of_classes, axis=1)
    return y


def knn_cross_val_score(X, y, k_list, score='accuracy', cv=None, **kwargs):
    y = y.astype(int)
    num_of_classes = np.amax(y) + 1
    if not cv:
        cv = kfold(y.size)
    classifier = KNNClassifier(k=k_list[-1], **kwargs)
    score_val = {}
    for k in k_list:
        score_val[k] = np.empty(len(cv))
    for i, split in enumerate(cv):
        classifier.fit(X[split[0]], y[split[0]])
        if classifier.weights:
            kneighbors = classifier.find_kneighbors(X[split[1]])
        else:
            kneighbors = classifier.find_kneighbors(X[split[1]], return_distance=False)
        for k in k_list:
            if classifier.weights:
                y_predicted = cross_val_predict(classifier, num_of_classes,
                                                (kneighbors[0][:, :k], kneighbors[1][:, :k]))
            else:
                y_predicted = cross_val_predict(classifier, num_of_classes, kneighbors[:, :k])
            score_val[k][i] = calculate_score(y[split[1]], y_predicted, score)
    return score_val
