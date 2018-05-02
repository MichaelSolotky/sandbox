import numpy as np
from collections import Counter
from sklearn.base import BaseEstimator


def compute_bias_variance(regressor, dependence_fun, x_generator=np.random.uniform, noise_generator=np.random.uniform,
                          sample_size=300, samples_num=300, objects_num=200, seed=1234):
    """
    После генерации всех необходимых объектов, должна вызываться функция compute_bias_variance_fixed_samples.

    Рекомендации:
    * Создайте вектор объектов для оценивания интеграла по $x$, затем вектор зашумленных правильных ответов.
      Оцените мат. ожидание шума с помощью генерации отдельной шумовой выборки длины objects_num.
    * Проверить правильность реализации можно на примерах, которые разбирались на семинаре и в домашней работе.

    :param regressor: объект sklearn-класса, реализующего регрессионный алгоритм (например, DecisionTreeRegressor,
     LinearRegression, Lasso, RandomForestRegressor ...)
    :param dependence_fun: функция, задающая истинную зависимость в данных. Принимает на вход вектор и возвращает вектор
     такой же длины. Примеры: np.sin, lambda x: x**2
    :param x_generator: функция, генерирующая одномерную выборку объектов и имеющая параметр size (число объектов в
     выборке). По умолчанию np.random.uniform
    :param noise_generator: функция, генерирующая одномерную выборку шумовых компонент (по одной на каждый объект) и
     имеющая параметр size (число объектов в выборке). По умолчанию np.random.uniform
    :param sample_size: число объектов в выборке
    :param samples_num: число выборок, которые нужно сгенерировать, чтобы оценить интеграл по X
    :param objects_num: число объектов, которые нужно сгенерировать, чтобы оценить интеграл по x
    :param seed: seed для функции np.random.seed

    :return bias: смещение алгоритма regressor (число)
    :return variance: разброс алгоритма regressor (число)
    """
    np.random.seed(seed)
    samples = x_generator(size=(samples_num, sample_size))
    objects = x_generator(size=objects_num)
    noise = noise_generator(size=(samples_num, sample_size))
    noise_mean = np.sum(noise) / noise.size
    return compute_bias_variance_fixed_samples(regressor, dependence_fun, samples,
                                               objects, noise, noise_mean)


def compute_bias_variance_fixed_samples(regressor, dependence_fun, samples, objects, noise, mean_noise):
    """
    В качестве допущения, будем оценивать $E_X\left[\mu(X)\right](x)$ как средний ответ на $x$ из samples_num
    алгоритмов, обученных на своих подвыборках $X$

    Рекомендации:
    * $\mathbb{E}[y|x]$ оценивается как сумма правильного ответа на объекте и мат. ожидания шума
      $\mathbb{E}_X [\mu(X)]$ оценивается как в предыдущей задаче: нужно обучить regressor на samples_num выборках длины
       sample_size и усреднить предсказания на сгенерированных ранее объектах.

    :param regressor: объект sklearn-класса, реализующего регрессионный алгоритм (например, DecisionTreeRegressor,
     LinearRegression, Lasso, RandomForestRegressor ...)
    :param dependence_fun: функция, задающая истинную зависимость в данных. Принимает на вход вектор и возвращает вектор
     такой же длины. Примеры: np.sin, lambda x: x**2
    :param samples: samples_num выборк длины sample_size для оценки интеграла по X
    :param objects: objects_num объектов для оценки интеграла по x
    :param noise: шумовая компонента размерности (samples_num, sample_size)
    :param mean_noise: среднее шумовой компоненты

    :return bias: смещение алгоритма regressor (число)
    :return variance: разброс алгоритма regressor (число)
    """
    y_train = dependence_fun(samples) + noise
    y_test = dependence_fun(objects) + mean_noise
    regressors_predictions = np.empty((samples.shape[0], objects.size))
    for i, (sample, answer) in enumerate(zip(samples, y_train)):
        regressor.fit(sample[:, np.newaxis], answer)
        regressors_predictions[i] = regressor.predict(objects[:, np.newaxis])
    bias = np.mean((regressors_predictions.mean(axis=0) - y_test)**2)
    tmp = regressors_predictions - regressors_predictions.mean(axis=0)[np.newaxis, :]
    variance = np.mean(tmp**2)
    return bias, variance


def calc_probas_for_information_criterion(splitting_mask, class_mask):
    tmp = (splitting_mask & class_mask).sum(axis=1)
    return tmp / splitting_mask.sum(axis=1)


def find_best_split(feature_vector, target_vector):
    """
    Под критерием Джини здесь подразумевается следующая функция:
    $$Q(R) = -\frac {|R_l|}{|R|}H(R_l) -\frac {|R_r|}{|R|}H(R_r)$$,
    $R$ — множество объектов, $R_l$ и $R_r$ — объекты, попавшие в левое и правое поддерево,
     $H(R) = 1-p_1^2-p_0^2$, $p_1$, $p_0$ — доля объектов класса 1 и 0 соответственно.

    Указания:
    * Пороги, приводящие к попаданию в одно из поддеревьев пустого множества объектов, не рассматриваются.
    * В качестве порогов, нужно брать среднее двух сосдених (при сортировке) значений признака
    * Поведение функции в случае константного признака может быть любым.
    * При одинаковых приростах Джини нужно выбирать минимальный сплит.
    * За наличие в функции циклов балл будет снижен. Векторизуйте! :)

    :param feature_vector: вещественнозначный вектор значений признака
    :param target_vector: вектор классов объектов,  len(feature_vector) == len(target_vector)

    :return thresholds: отсортированный по возрастанию вектор со всеми возможными порогами, по которым объекты можно
     разделить на две различные подвыборки, или поддерева
    :return ginis: вектор со значениями критерия Джини для каждого из порогов в thresholds len(ginis) == len(thresholds)
    :return threshold_best: оптимальный порог (число)
    :return gini_best: оптимальное значение критерия Джини (число)
    """
    arg = np.argsort(feature_vector)
    feature_vector = feature_vector[arg]
    target_vector = target_vector[arg]
    thresholds = (feature_vector + np.roll(feature_vector, 1))[1:] / 2
    # For each threshold find splitting mask
    # Example:
    # [[1 0 0 0]
    #  [1 1 0 0]
    #  [1 1 1 0]]
    left_objects_mask = feature_vector[np.newaxis, :] < thresholds[:, np.newaxis]
    right_objects_mask = ~left_objects_mask
    non_zero_lines = left_objects_mask.any(axis=1) & right_objects_mask.any(axis=1)
    thresholds = thresholds[non_zero_lines]
    left_objects_mask = feature_vector[np.newaxis, :] < thresholds[:, np.newaxis]
    right_objects_mask = ~left_objects_mask

    proportions_of_left_subsamples = np.mean(left_objects_mask, axis=1)
    proportions_of_right_subsamples = 1 - proportions_of_left_subsamples

    pos_class_mask = target_vector[np.newaxis, :]
    neg_class_mask = ~pos_class_mask
    p_0_left = calc_probas_for_information_criterion(left_objects_mask, neg_class_mask)
    p_1_left = calc_probas_for_information_criterion(left_objects_mask, pos_class_mask)
    p_0_right = calc_probas_for_information_criterion(right_objects_mask, neg_class_mask)
    p_1_right = calc_probas_for_information_criterion(right_objects_mask, pos_class_mask)

    left_ginis = -proportions_of_left_subsamples * (1 - p_0_left**2 - p_1_left**2)
    right_ginis = -proportions_of_right_subsamples * (1 - p_0_right**2 - p_1_right**2)
    ginis = left_ginis + right_ginis
    pos_best_gini = np.argmax(ginis)
    threshold_best = thresholds[pos_best_gini]
    gini_best = ginis[pos_best_gini]
    return thresholds, ginis, threshold_best, gini_best


class DecisionTree(BaseEstimator):
    def __init__(self, feature_types, max_depth=None, min_samples_split=None, min_samples_leaf=None):
        if np.any(list(map(lambda x: x != "real" and x != "categorical", feature_types))):
            raise ValueError("There is unknown feature type")

        self._tree = {}
        self.feature_types = feature_types
        self.max_depth = max_depth
        self.min_samples_split = min_samples_split
        self.min_samples_leaf = min_samples_leaf

    def _fit_node(self, sub_X, sub_y, node, cur_depth=1):
        # if np.all(sub_y != sub_y[0]):
        if np.all(sub_y == sub_y[0]):
            node["type"] = "terminal"
            node["class"] = sub_y[0]
            return

        feature_best, threshold_best, gini_best, split = None, None, None, None
        # for feature in range(1, sub_X.shape[1]):
        for feature in range(sub_X.shape[1]):
            feature_type = self.feature_types[feature]
            categories_map = {}

            if feature_type == "real":
                counts = Counter(sub_X[:, feature])
                if len(counts) == 1:
                    continue
                feature_vector = sub_X[:, feature]
            elif feature_type == "categorical":
                counts = Counter(sub_X[:, feature])
                if len(counts) == 1:
                    continue
                clicks = Counter(sub_X[sub_y == 1, feature])
                ratio = {}
                for key, current_count in counts.items():
                    if key in clicks:
                        current_click = clicks[key]
                    else:
                        current_click = 0
                    # ratio[key] = current_count / current_click
                    ratio[key] = current_click / current_count
                # sorted_categories = list(map(lambda x: x[1], sorted(ratio.items(), key=lambda x: x[1])))
                sorted_categories = list(map(lambda x: x[0], sorted(ratio.items(), key=lambda x: x[1])))
                categories_map = dict(zip(sorted_categories, list(range(len(sorted_categories)))))
                # feature_vector = np.array(map(lambda x: categories_map[x], sub_X[:, feature]))
                feature_vector = np.fromiter(map(lambda x: categories_map[x], sub_X[:, feature]), dtype=float)
            else:
                raise ValueError

            # if len(feature_vector) == 3:
            #    continue

            _, _, threshold, gini = find_best_split(feature_vector, sub_y)
            if gini_best is None or gini > gini_best:
                feature_best = feature
                gini_best = gini
                split = feature_vector < threshold

                if feature_type == "real":
                    threshold_best = threshold
                # elif feature_type == "Categorical":
                elif feature_type == "categorical":
                    threshold_best = list(map(lambda x: x[0],
                                              filter(lambda x: x[1] < threshold, categories_map.items())))
                else:
                    raise ValueError

        if feature_best is None:
            node["type"] = "terminal"
            # node["class"] = Counter(sub_y).most_common(1)
            node["class"] = Counter(sub_y).most_common(1)[0][0]
            return

        node["type"] = "nonterminal"
        node["feature_split"] = feature_best
        if self.feature_types[feature_best] == "real":
            node["threshold"] = threshold_best
        elif self.feature_types[feature_best] == "categorical":
            node["categories_split"] = threshold_best
        else:
            raise ValueError
        node["left_child"], node["right_child"] = {}, {}
        # print('split proportions =', np.mean(split), np.mean(~split),
        #      'current depth =', cur_depth, 'gini best =', gini_best)
        self._fit_node(sub_X[split], sub_y[split], node["left_child"], cur_depth + 1)
        # self._fit_node(sub_X[np.logical_not(split)], sub_y[split], node["right_child"])
        self._fit_node(sub_X[~split], sub_y[~split], node["right_child"], cur_depth + 1)

    def _predict_node(self, x, node):
        if node["type"] == "terminal":
            return node["class"]
        if self.feature_types[node["feature_split"]] == "real":
            if x[node["feature_split"]] < node["threshold"]:
                return self._predict_node(x, node["left_child"])
            else:
                return self._predict_node(x, node["right_child"])
        else:
            if x[node["feature_split"]] in node["categories_split"]:
                return self._predict_node(x, node["left_child"])
            else:
                return self._predict_node(x, node["right_child"])

    def fit(self, X, y):
        self._fit_node(X, y, self._tree)

    def predict(self, X):
        predicted = np.empty(X.shape[0])
        for i, x in enumerate(X):
            predicted[i] = self._predict_node(x, self._tree)
        return predicted
