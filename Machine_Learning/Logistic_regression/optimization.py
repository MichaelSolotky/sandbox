import oracles
import numpy as np
import time
from scipy import special
from sklearn import metrics


class GDClassifier:
    """
    Реализация метода градиентного спуска для произвольного
    оракула, соответствующего спецификации оракулов из модуля oracles.py
    """
    def __init__(self, loss_function, step_alpha=1, step_beta=0,
                 tolerance=1e-5, max_iter=1000, fit_intercept=False, **kwargs):
        """
        loss_function - строка, отвечающая за функцию потерь классификатора. 
        Может принимать значения:
        - 'binary_logistic' - бинарная логистическая регрессия
        - 'multinomial_logistic' - многоклассовая логистическая регрессия

        step_alpha - float, параметр выбора шага из текста задания

        step_beta- float, параметр выбора шага из текста задания

        tolerance - точность, по достижении которой, необходимо прекратить оптимизацию.
        Необходимо использовать критерий выхода по модулю разности соседних значений функции:
        если (f(x_{k+1}) - f(x_{k})) < tolerance: то выход 

        max_iter - максимальное число итераций     

        **kwargs - аргументы, необходимые для инициализации
        """
        if loss_function != 'multinomial_logistic' and loss_function != 'binary_logistic':
            raise ValueError('loss function can be either multinomial_logistic' + \
                'or binary_logistic, but ' + str(loss_function) + ' given')
        self.loss_function = loss_function
        self.step_alpha = step_alpha
        self.step_beta = step_beta
        self.tolerance = tolerance
        self.max_iter = max_iter
        self.fit_intercept = fit_intercept
        self.oracles_kwargs = kwargs

    def fit(self, X, y, w_0=None, trace=False, X_test=None, y_test=None):
        """
        Обучение метода по выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array
        w_0 - начальное приближение в методе
        trace - переменная типа bool

        Если trace = True, то метод должен вернуть словарь history, содержащий информацию 
        о поведении метода. Длина словаря history = количество итераций + 1 (начальное приближение)

        history['time']: list of floats, содержит интервалы времени между двумя итерациями метода
        history['func']: list of floats, содержит значения функции на каждой итерации
        (0 для самой первой точки)
        """
        num_of_classes = int(np.max(y) + 1)
        if self.loss_function == 'binary_logistic':
            if w_0 is None:
                w_0 = np.zeros(X.shape[1])
            self.bias = 0.
            self.oracle = oracles.BinaryLogistic(**self.oracles_kwargs)
        else:
            if w_0 is None:
                w_0 = np.zeros((num_of_classes, X.shape[1]))
            self.bias = np.zeros(num_of_classes)
            if not 'class_number' in self.oracles_kwargs:
                self.oracle = oracles.MulticlassLogistic(class_number=num_of_classes,
                                                         **self.oracles_kwargs)
            else:
                self.oracle = oracles.MulticlassLogistic(**self.oracles_kwargs)
        self.w = np.copy(w_0)

        if trace:
            history = {}
            start_time = time.time()
            history['time'] = [0]
            history['func'] = [self.oracle.func(X, y, self.w, self.bias)]
            if not X_test is None:
                y_pred = self.predict(X_test)
                acc = metrics.accuracy_score(y_test, y_pred)
                history['accuracy'] = [acc]
            
        for k in range(1, self.max_iter + 1):
            learning_rate = self.step_alpha / pow(k, self.step_beta)
            if self.fit_intercept:
                new_bias = self.bias - learning_rate * self.oracle.grad(X, y, self.w,
                                                            calculate_for_bias=True,
                                                            bias=self.bias)
            self.w -= learning_rate * self.oracle.grad(X, y, self.w, bias=self.bias)
            if self.fit_intercept:
                self.bias = new_bias
            if trace:
                history['time'].append(time.time() - start_time)
                history['func'].append(self.oracle.func(X, y, self.w, bias=self.bias))
                if not X_test is None:
                    y_pred = self.predict(X_test)
                    accuracy = metrics.accuracy_score(y_test, y_pred)
                    history['accuracy'].append(accuracy)
                if abs(history['func'][-2] - history['func'][-1]) < self.tolerance:
                    break
        if trace:
            return history

    def predict_proba(self, X):
        """
        Получение вероятностей принадлежности X к классу k

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        return: двумерной numpy array, [i, k] значение соответветствует вероятности
        принадлежности i-го объекта к классу k
        """
        if self.loss_function == 'binary_logistic':
            probas = special.expit(X.dot(self.w) + self.bias)[:, np.newaxis]
            return np.hstack((1 - probas, probas))
        else:
            probas = self.oracle.softmax(X, self.w, self.bias)
            return probas

    def predict(self, X):
        """
        Получение меток ответов на выборке X

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        return: одномерный numpy array с предсказаниями
        """
        if self.loss_function == 'binary_logistic':
            return np.sign(X.dot(self.w) + self.bias)
        else:
            return np.argmax(self.oracle.softmax(X, self.w, self.bias), axis=1)

    def get_objective(self, X, y):
        """
        Получение значения целевой функции на выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array

        return: float
        """
        #return self.oracle.func(X, y, self.w, self.bias)
        return self.oracle.func(X, y, self.w)

    def get_gradient(self, X, y):
        """
        Получение значения градиента функции на выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array

        return: numpy array, размерность зависит от задачи
        """
        #return self.oracle.func(X, y, self.w, self.bias)
        return self.oracle.func(X, y, self.w)

    def get_weights(self):
        """
        Получение значения весов функционала
        """
        return self.w

class SGDClassifier(GDClassifier):
    """
    Реализация метода стохастического градиентного спуска для произвольного
    оракула, соответствующего спецификации оракулов из модуля oracles.py
    """

    def __init__(self, loss_function, batch_size=1, step_alpha=1, step_beta=0,
                 tolerance=1e-5, max_iter=1000, random_seed=153,
                 fit_intercept=False, **kwargs):
        """
        loss_function - строка, отвечающая за функцию потерь классификатора.
        Может принимать значения:
        - 'binary_logistic' - бинарная логистическая регрессия
        - 'multinomial_logistic' - многоклассовая логистическая регрессия

        batch_size - размер подвыборки, по которой считается градиент

        step_alpha - float, параметр выбора шага из текста задания

        step_beta- float, параметр выбора шага из текста задания

        tolerance - точность, по достижении которой, необходимо прекратить оптимизацию
        Необходимо использовать критерий выхода по модулю разности соседних значений функции:
        если (f(x_{k+1}) - f(x_{k})) < tolerance: то выход 

        max_iter - максимальное число итераций

        random_seed - в начале метода fit необходимо вызвать np.random.seed(random_seed).
        Этот параметр нужен для воспроизводимости результатов на разных машинах.

        **kwargs - аргументы, необходимые для инициализации
        """
        super().__init__(loss_function, step_alpha, step_beta,
            tolerance, max_iter, fit_intercept, **kwargs)
        self.seed = random_seed
        self.batch_size = batch_size

    def fit(self, X_train, y_train, w_0=None, trace=False, log_freq=1, X_test=None, y_test=None):
        """
        Обучение метода по выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w_0 - начальное приближение в методе

        Если trace = True, то метод должен вернуть словарь history, содержащий информацию 
        о поведении метода. Если обновлять history после каждой итерации, метод перестанет 
        превосходить в скорости метод GD. Поэтому, необходимо обновлять историю метода лишь
        после некоторого числа обработанных объектов в зависимости от приближённого номера эпохи.
        Приближённый номер эпохи:
            {количество объектов, обработанных методом SGD} / {количество объектов в выборке}

        log_freq - float от 0 до 1, параметр, отвечающий за частоту обновления. 
        Обновление должно проиходить каждый раз, когда разница между двумя значениями приближённого номера эпохи
        будет превосходить log_freq.

        history['epoch_num']: list of floats, в каждом элементе списка будет записан приближённый номер эпохи:
        history['time']: list of floats, содержит интервалы времени между двумя соседними замерами
        history['func']: list of floats, содержит значения функции после текущего приближённого номера эпохи
        history['weights_diff']: list of floats, содержит квадрат нормы разности векторов весов с соседних замеров
        (0 для самой первой точки)
        """
        train_set_len = X_train.shape[0]
        num_of_classes = int(np.max(y_train) + 1)
        if self.loss_function == 'binary_logistic':
            if w_0 is None:
                w_0 = np.zeros(X_train.shape[1])
            self.bias = 0.
            self.oracle = oracles.BinaryLogistic(**self.oracles_kwargs)
        else:
            if w_0 is None:
                w_0 = np.zeros((num_of_classes, X_train.shape[1]))
            self.bias = np.zeros(num_of_classes)
            if not 'class_number' in self.oracles_kwargs:
                self.oracle = oracles.MulticlassLogistic(class_number=num_of_classes,
                                                         **self.oracles_kwargs)
            else:
                self.oracle = oracles.MulticlassLogistic(**self.oracles_kwargs)
        self.w = np.copy(w_0)

        if trace:
            history = {}
            history['epoch_num'] = [0]
            history['time'] = [0]
            history['func'] = [float('inf'), self.oracle.func(X_train, y_train, self.w, self.bias)]
            history['weights_diff'] = [0]
            start_time = time.time()
            if not X_test is None:
                y_pred = self.predict(X_test)
                accuracy = metrics.accuracy_score(y_test, y_pred)
                history['accuracy'] = [accuracy]

        renew_counter = 1
        epoch_num = 0
        indices_cur_position = 0
        indices = np.arange(train_set_len)
        np.random.shuffle(indices)
        indices = np.hstack((indices, indices))
        for k in range(1, self.max_iter + 1):
            learning_rate = self.step_alpha / pow(k, self.step_beta)
            random_indices = indices[indices_cur_position:indices_cur_position + self.batch_size]
            if self.fit_intercept:
                new_bias = self.bias - learning_rate * self.oracle.grad(X_train[random_indices],
                                                                        y_train[random_indices],
                                                                        self.w,
                                                                        calculate_for_bias=True,
                                                                        bias=self.bias)
            weights_diff = learning_rate * self.oracle.grad(X_train[random_indices],
                                                            y_train[random_indices],
                                                            self.w, bias=self.bias)
            self.w -= weights_diff
            if self.fit_intercept:
                self.bias = new_bias

            indices_cur_position += self.batch_size
            if indices_cur_position >= train_set_len:
                indices_cur_position -= train_set_len

            if trace:
                epoch_num += self.batch_size / y_train.size
                if epoch_num >= renew_counter * log_freq:
                    history['epoch_num'].append(epoch_num)
                    history['func'].append(self.oracle.func(X_train, y_train, self.w, bias=self.bias))
                    renew_counter += 1
                    history['time'].append(time.time() - start_time)
                    history['weights_diff'].append(np.sum(weights_diff ** 2))
                    if not X_test is None:
                        y_pred = self.predict(X_test)
                        accuracy = metrics.accuracy_score(y_test, y_pred)
                        history['accuracy'].append(accuracy)
                if abs(history['func'][-2] - history['func'][-1]) < self.tolerance:
                    break
        if trace:
            history['func'] = history['func'][1:]
            return history