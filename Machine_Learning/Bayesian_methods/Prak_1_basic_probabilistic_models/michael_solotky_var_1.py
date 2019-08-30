import numpy as np
from scipy.stats import binom, poisson

# There should be no main() in this file!!! 
# Nothing should start running when you import this file somewhere.
# You may add other supporting functions to this file.
#
# Important rules:
# 1) Function pa_bc must return tensor which has dimensions (#a x #b x #c),
#    where #v is a number of different values of the variable v.
#    For input variables #v = how many input values of this variable you give to the function.
#    For output variables #v = number of all possible values of this variable.
#    Ex. for pb_a: #b = bmax-bmin+1,   #a is arbitrary.
# 2) Random variables in function names must be written in alphabetic order
#    e.g. pda_cb is an improper function name (pad_bc must be used instead)
# 3) Single dimension must be explicitly stated:
#    if you give only one value of a variable a to the function pb_a, i.e. #a=1, 
#    then the function pb_a must return tensor of shape (#b, 1), not (#b,).
#
# The format of all the functions for distributions is the following:
# Inputs:
# params - dictionary with keys 'amin', 'amax', 'bmin', 'bmax', 'p1', 'p2', 'p3'
# model - model number, number from 1 to 4
# all other parameters - values of the conditions (variables a, b, c, d).
#                        Numpy vectors of size (k,), where k is an arbitrary number.
#                        For variant 3: c and d must be numpy arrays of size (k,N),
#                        where N is a number of lectures.
# Outputs:
# prob, val
# prob - probabilities for different values of the output variable with different input conditions
#        prob[i,...] = p(v=val[i]|...)
# val - support of a distribution, numpy vector of size (#v,) for variable v
#
# Example 1:
#    Function pc_ab - distribution p(c|a,b)
#    Input: a of size (k_a,) and b of size (k_b,)
#    Result: prob of size (cmax-cmin+1,k_a,k_b), val of size (cmax-cmin+1,) 
#
# Example 2 (for variant 3):
#    Function pb_ad - distribution p(b|a,d_1,...,d_N)
#    Input: a of size (k_a,) and d of size (k_d,N)
#    Result: prob of size (bmax-bmin+1,k_a,k_d), val of size (bmax-bmin+1,)
#
# The format the generation function from variant 3 is the following:
# Inputs:
# N - how many points to generate
# all other inputs have the same format as earlier
# Outputs:
# d - generated values of d, numpy array of size (N,#a,#b)

# In variant 1 the following functions are required:
def pa(params, model):
    res1 = np.ones(params["amax"] - params["amin"] + 1) / (params["amax"] - params["amin"] + 1)
    res2 = np.arange(params["amin"], params["amax"] + 1)
    return res1, res2

def pb(params, model):
    res1 = np.ones(params["bmax"] - params["bmin"] + 1) / (params["bmax"] - params["bmin"] + 1)
    res2 = np.arange(params["bmin"], params["bmax"] + 1)
    return res1, res2

def pc(params, model):
    proba_b, b = pb(params, model)
    proba_c_b, _ = pc_b(b, params, model)
    res1 = np.sum(proba_c_b * proba_b[0], axis=1)
    res2 = np.arange(params["amax"] + params["bmax"] + 1)
    return res1, res2

def pd(params, model):
    val_c = np.arange(params["amax"] + params["bmax"] + 1)
    val_d = np.arange((params["amax"] + params["bmax"]) * 2 + 1)
    res1 = np.sum(pd_c(val_c, params, model)[0] * pc(params, model)[0][np.newaxis, :], axis=1)
    res2 = val_d
    return res1, res2

def pc_a(a, params, model):
    b = np.arange(params["bmin"], params["bmax"] + 1)
    proba_c_ab, _ = pc_ab(a, b, params, model)
    proba_b, _ = pb(params, model)
    res1 = np.sum(proba_c_ab * proba_b[0], axis=2)
    res2 = np.arange(params["amax"] + params["bmax"] + 1)
    return res1, res2

def pc_b(b, params, model):
    a = np.arange(params["amin"], params["amax"] + 1)
    proba_c_ab, _ = pc_ab(a, b, params, model)
    proba_a, _ = pa(params, model)
    res1 = np.sum(proba_c_ab * pa(params, model)[0][0], axis=1)
    res2 = np.arange(params["amax"] + params["bmax"] + 1)
    return res1, res2

def pc_d(d, params, model):
    c = np.arange(params["amax"] + params["bmax"] + 1)
    proba_cd = np.swapaxes(pd_c(c, params, model)[0][d, :], 0, 1) * pc(params, model)[0][:, np.newaxis]
    res1 = proba_cd / pd(params, model)[0][np.newaxis, d]
    res2 = c
    return res1, res2

def pc_ab(a, b, params, model):
    p1 = params["p1"]
    p2 = params["p2"]
    val = np.arange(params["amax"] + params["bmax"] + 1)
    proba_c_ab = np.zeros((val.size, a.size, b.size))
    if model == 1:
        proba_a = binom.pmf(np.arange(val.size)[np.newaxis, :], a[:, np.newaxis], p1)
        proba_b = binom.pmf(np.arange(val.size - 1, -1, -1)[:, np.newaxis], b[np.newaxis, :], p2)
    else:
        proba_a = poisson.pmf(np.arange(val.size)[np.newaxis, :], a[:, np.newaxis] * p1)
        proba_b = poisson.pmf(np.arange(val.size - 1, -1, -1)[:, np.newaxis], b[np.newaxis, :] * p2)
    for k in val:
        proba_c_ab[k] = np.dot(proba_a[:, :k + 1], proba_b[-k - 1:, :])
    return proba_c_ab, val

def pc_abd(a, b, d, params, model):
    c = np.arange(params["amax"] + params["bmax"] + 1)
    proba_c_ab, _ = pc_ab(a, b, params, model)
    proba_d_c = pd_c(c, params, model)[0][d]
    proba_d_ab = np.tensordot(proba_d_c, proba_c_ab, axes=(1, 0))
    proba_cd_ab = np.swapaxes(proba_d_c, 0, 1)[np.newaxis, np.newaxis, :, :] * \
                  np.swapaxes(np.swapaxes(proba_c_ab, 0, 1), 1, 2)[:, :, :, np.newaxis]
    proba_c_abd = proba_cd_ab / np.swapaxes(np.swapaxes(proba_d_ab, 0, 1), 1, 2)[:, :, np.newaxis, :]
    res1 = np.swapaxes(np.swapaxes(proba_c_abd, 2, 1), 1, 0)
    res2 = c
    return res1, res2

def pd_c(c, params, model):
    d = np.arange((params["amax"] + params["bmax"]) * 2 + 1)
    res1 = binom.pmf(k=d[:, np.newaxis]-c[np.newaxis, :], n=c, p=params['p3'])
    res2 = d
    return res1, res2
