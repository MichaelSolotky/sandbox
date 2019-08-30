def grad_finite_diff(function, w, eps=1e-8):
    """
    Возвращает численное значение градиента, подсчитанное по следующией формуле:
        result_i := (f(w + eps * e_i) - f(w)) / eps,
        где e_i - следующий вектор:
        e_i = (0, 0, ..., 0, 1, 0, ..., 0)
                          >> i <<
    """
    res = np.empty(w.shape)
    basis_vector = np.zero(w.shape)
    l = len(w.shape)
    if l == 1:
        for i in range(w.size):
            basis_vector[i] = 1
            res[i] = (function(w + eps * basis_vector) - function(w)) / eps
            basis_vector[i] = 0
    elif l == 2:
        for i in range(w.shape[0]):
            for j in range(w.shape[1]):
                basis_vector[i][j] = 1
                res[i][j] = (function(w + eps * basis_vector) - function(w)) / eps
                basis_vector[i][j] = 0
    elif l == 0:
        raise TypeError('Empty input array w')
    else:
        raise TypeError('can\'t process input array w with more than 2 dimentions')
    return res