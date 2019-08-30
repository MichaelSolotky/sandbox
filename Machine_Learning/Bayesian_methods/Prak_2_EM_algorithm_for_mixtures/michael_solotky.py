import numpy as np
from scipy.signal import fftconvolve


def calculate_log_probability(X, F, B, s):
    """
    Calculates log p(X_k|d_k,F,B,s) for all images X_k in X and
    all possible displacements d_k.

    Parameters
    ----------
    X : array, shape (H, W, K)
        K images of size H x W.
    F : array, shape (h, w)
        Estimate of villain's face.
    B : array, shape (H, W)
        Estimate of background.
    s : float
        Estimate of standard deviation of Gaussian noise.

    Returns
    -------
    ll : array, shape (H-h+1, W-w+1, K)
        ll[dh,dw,k] - log-likelihood of observing image X_k given
        that the villain's face F is located at displacement (dh, dw)
    """
    H, W, K = X.shape
    h, w = F.shape
    result = np.zeros((H - h + 1, W - w + 1, K))
    for i in range(H - h + 1):
        for j in range(W - w + 1):
            curr_image = B.copy()
            curr_image[i:i + h, j:j + w] = F
            result[i, j] -= ((X - curr_image[:, :, np.newaxis]) ** 2).sum(axis=(0, 1))
    result /= 2 * s ** 2
    result -= 0.5 * W * H * np.log(2 * np.pi * s ** 2)
    return result


def calculate_lower_bound(X, F, B, s, A, q, use_MAP=False):
    """
    Calculates the lower bound L(q,F,B,s,A) for the marginal log likelihood.

    Parameters
    ----------
    X : array, shape (H, W, K)
        K images of size H x W.
    F : array, shape (h, w)
        Estimate of villain's face.
    B : array, shape (H, W)
        Estimate of background.
    s : float
        Estimate of standard deviation of Gaussian noise.
    A : array, shape (H-h+1, W-w+1)
        Estimate of prior on displacement of face in any image.
    q : array
        If use_MAP = False: shape (H-h+1, W-w+1, K)
            q[dh,dw,k] - estimate of posterior of displacement (dh,dw)
            of villain's face given image Xk
        If use_MAP = True: shape (2, K)
            q[0,k] - MAP estimates of dh for X_k
            q[1,k] - MAP estimates of dw for X_k
    use_MAP : bool, optional
        If true then q is a MAP estimates of displacement (dh,dw) of
        villain's face given image Xk.

    Returns
    -------
    L : float
        The lower bound L(q,F,B,s,A) for the marginal log likelihood.
    """
    K = X.shape[2]
    ll = calculate_log_probability(X, F, B, s)
    if use_MAP:
        log_joint_proba = ll[q[0], q[1], np.arange(K)] + np.log(A[q[0], q[1]])
        entropy = 0.
    else:
        log_joint_proba = ll + np.log(A)[:, :, np.newaxis]
        log_joint_proba *= q
        entropy = -(np.log(q) * q).sum()
    lower_bound = log_joint_proba.sum() + entropy
    return lower_bound


def run_e_step(X, F, B, s, A, use_MAP=False):
    """
    Given the current esitmate of the parameters, for each image Xk
    esitmates the probability p(d_k|X_k,F,B,s,A).

    Parameters
    ----------
    X : array, shape(H, W, K)
        K images of size H x W.
    F  : array_like, shape(h, w)
        Estimate of villain's face.
    B : array shape(H, W)
        Estimate of background.
    s : scalar, shape(1, 1)
        Eestimate of standard deviation of Gaussian noise.
    A : array, shape(H-h+1, W-w+1)
        Estimate of prior on displacement of face in any image.
    use_MAP : bool, optional,
        If true then q is a MAP estimates of displacement (dh,dw) of
        villain's face given image Xk.

    Returns
    -------
    q : array
        If use_MAP = False: shape (H-h+1, W-w+1, K)
            q[dh,dw,k] - estimate of posterior of displacement (dh,dw)
            of villain's face given image Xk
        If use_MAP = True: shape (2, K)
            q[0,k] - MAP estimates of dh for X_k
            q[1,k] - MAP estimates of dw for X_k
    """
    H, W, K = X.shape
    h, w = F.shape
    ll = calculate_log_probability(X, F, B, s)
    if use_MAP:
        non_empty_components_mask = A > 1e-6
        empty_components_mask = ~non_empty_components_mask
        log_joint = np.empty((H - h + 1, W - w + 1, K))
        log_joint[empty_components_mask] = float("-inf")
        log_joint[non_empty_components_mask] = ll[non_empty_components_mask] + \
                np.log(A[non_empty_components_mask])[:, np.newaxis]
        q = np.empty((2, K), dtype=int)
        arg_max = log_joint.reshape((-1, K)).argmax(axis=0)
        q[0, :] = arg_max // (H - h + 1)
        q[1, :] = arg_max % (H - h + 1)
        return q
    else:
        log_joint = ll + np.log(A)[..., np.newaxis]
        log_joint -= log_joint.max(axis=(0, 1))[..., :]
        joint = np.exp(log_joint)
        evidence = joint.sum(axis=(0, 1))
        posterior = joint / evidence[..., :]
        return posterior


def run_m_step(X, q, h, w, use_MAP=False):
    """
    Estimates F,B,s,A given esitmate of posteriors defined by q.

    Parameters
    ----------
    X : array, shape(H, W, K)
        K images of size H x W.
    q :
        if use_MAP = False: array, shape (H-h+1, W-w+1, K)
           q[dh,dw,k] - estimate of posterior of displacement (dh,dw)
           of villain's face given image Xk
        if use_MAP = True: array, shape (2, K)
            q[0,k] - MAP estimates of dh for X_k
            q[1,k] - MAP estimates of dw for X_k
    h : int
        Face mask height.
    w : int
        Face mask width.
    use_MAP : bool, optional
        If true then q is a MAP estimates of displacement (dh,dw) of
        villain's face given image Xk.

    Returns
    -------
    F : array, shape (h, w)
        Estimate of villain's face.
    B : array, shape (H, W)
        Estimate of background.
    s : float
        Estimate of standard deviation of Gaussian noise.
    A : array, shape (H-h+1, W-w+1)
        Estimate of prior on displacement of face in any image.
    """
    H, W, K = X.shape
    # Update A
    if use_MAP:
        q_tmp = np.zeros((H - h + 1, W - w + 1, K), dtype=float)
        q_tmp[q[0], q[1], np.arange(K)] = 1
        A = q_tmp.sum(axis=2) / K
    else:
        A = q.sum(axis=2) / K
    A[A < 1e-7] = 1e-7 # there can be zeros or very small values sometimes
    # Update B
    if use_MAP:
        B = np.zeros((H, W))
        background_counts = np.zeros((H, W), dtype=int)
        for k in range(K):
            background_mask = np.ones((H, W), dtype=bool)
            background_mask[q[0, k]:q[0, k] + h, q[1, k]:q[1, k] + w].fill(False)
            background_counts += background_mask
            B[background_mask] += X[:, :, k][background_mask]
        non_zero_counts = background_counts != 0
        B[non_zero_counts] /= background_counts[non_zero_counts]
        # encode values that shouldn't be changed during update
        B[~non_zero_counts].fill(float("inf"))
    else:
        B = X.sum(axis=2)
        B_correction = np.full((H, W), K, dtype=float)
        for i in range(H - h + 1):
            for j in range(W - w + 1):
                B[i:i + h, j:j + w] -= (X[i:i + h, j:j + w] * q[i, j][..., :]).sum(axis=2)
                B_correction[i:i + h, j:j + w] -= q[i, j].sum()
        B /= B_correction
    # Update F
    F = np.zeros((h, w))
    if use_MAP:
        for k in range(K):
            F += X[q[0, k]:q[0, k] + h, q[1, k]:q[1, k] + w, k]
    else:
        for k in range(K):
            F += fftconvolve(X[:, :, k], q[::-1, ::-1, k], mode='valid')
    F /= K
    # Update s^2
    s2 = 0.
    if use_MAP:
        for k in range(K):
            frame_mask = np.zeros((H, W), dtype=bool)
            frame_mask[q[0, k]:q[0, k] + h, q[1, k]:q[1, k] + w].fill(1)
            background_mask = ~frame_mask
            s2 += ((X[background_mask].reshape((-1, K))[:, k] - B[background_mask]) ** 2).sum()
            s2 += ((X[frame_mask].reshape((h, w, K))[:, :, k] - F) ** 2).sum()
    else:
        for i in range(H - h + 1):
            for j in range(W - w + 1):
                curr_image = B.copy()
                curr_image[i:i + h, j:j + w] = F
                se = ((X - curr_image[:, :, np.newaxis]) ** 2).sum(axis=(0, 1))
                s2 += (q[i, j] * se).sum()
    s2 /= (K * H * W)
    s = np.sqrt(s2)
    return F, B, s, A


def run_EM(X, h, w, F=None, B=None, s=None, A=None, tolerance=0.001,
           max_iter=50, use_MAP=False):
    """
    Runs EM loop until the likelihood of observing X given current
    estimate of parameters is idempotent as defined by a fixed
    tolerance.

    Parameters
    ----------
    X : array, shape (H, W, K)
        K images of size H x W.
    h : int
        Face mask height.
    w : int
        Face mask width.
    F : array, shape (h, w), optional
        Initial estimate of villain's face.
    B : array, shape (H, W), optional
        Initial estimate of background.
    s : float, optional
        Initial estimate of standard deviation of Gaussian noise.
    A : array, shape (H-h+1, W-w+1), optional
        Initial estimate of prior on displacement of face in any image.
    tolerance : float, optional
        Parameter for stopping criterion.
    max_iter  : int, optional
        Maximum number of iterations.
    use_MAP : bool, optional
        If true then after E-step we take only MAP estimates of displacement
        (dh,dw) of villain's face given image Xk.

    Returns
    -------
    F, B, s, A : trained parameters.
    LL : array, shape(number_of_iters,)
        L(q,F,B,s,A) after each EM iteration (1 iteration = 1 e-step + 1 m-step); 
        number_of_iters is actual number of iterations that was done.
    """
    H, W, K = X.shape
    if F is None:
        F = np.zeros((h, w))
    if B is None:
        B = np.zeros((H, W))
    if s is None:
        s = 1.
    if A is None:
        A = np.empty((H - h + 1, W - w + 1))
        A.fill(1 / ((H - h + 1) * (W - w + 1)))
    lb = []
    prev_lower_bound = float("-inf")
    if use_MAP:
        for i in range(max_iter):
            q = run_e_step(X, F, B, s, A, use_MAP)
            B_prev = B
            F, B, s, A = run_m_step(X, q, h, w, use_MAP)
            inf_mask = B == float("inf")
            B[inf_mask] = B_prev[inf_mask]
            lower_bound = calculate_lower_bound(X, F, B, s, A, q, use_MAP)
            if lower_bound - prev_lower_bound < tolerance:
                break
            prev_lower_bound = lower_bound
            lb.append(lower_bound)
    else:
        for i in range(max_iter):
            q = run_e_step(X, F, B, s, A, use_MAP)
            F, B, s, A = run_m_step(X, q, h, w, use_MAP)
            lower_bound = calculate_lower_bound(X, F, B, s, A, q, use_MAP)
            if lower_bound - prev_lower_bound < tolerance:
                break
            prev_lower_bound = lower_bound
            lb.append(lower_bound)
    return F, B, s, A, np.array(lb)


def run_EM_with_restarts(X, h, w, tolerance=0.001, max_iter=50, use_MAP=False,
                         n_restarts=10):
    """
    Restarts EM several times from different random initializations
    and stores the best estimate of the parameters as measured by
    the L(q,F,B,s,A).

    Parameters
    ----------
    X : array, shape (H, W, K)
        K images of size H x W.
    h : int
        Face mask height.
    w : int
        Face mask width.
    tolerance, max_iter, use_MAP : optional parameters for EM.
    n_restarts : int
        Number of EM runs.

    Returns
    -------
    F : array,  shape (h, w)
        The best estimate of villain's face.
    B : array, shape (H, W)
        The best estimate of background.
    s : float
        The best estimate of standard deviation of Gaussian noise.
    A : array, shape (H-h+1, W-w+1)
        The best estimate of prior on displacement of face in any image.
    L : float
        The best L(q,F,B,s,A).
    """
    F_init = np.zeros((h, w))
    B_init = np.zeros((H, W))
    s_init = 1.
    A_init = np.empty((H - h + 1, W - w + 1))
    A_init.fill(1 / ((H - h + 1) * (W - w + 1)))
    F_best, B_best, s_best, A_best = F_init, B_init, s_init, A_init
    best_lower_bound = float("-inf")
    for i in range(n_restarts):
        F, B, s, A, lb = run_EM(X, h, w, F_init, B_init, s_init, A_init, tolerance, max_iter, use_MAP)
        if lb[-1] > best_lower_bound:
            F_best, B_best, s_best, A_best, best_lower_bound = F, B, s, A, lb[-1]
    return F_best, B_best, s_best, A_best, best_lower_bound