import numpy as np
import math


class UnconditionalProblem():
    # J(u) = a * ||u||^b + c * <Du, u> + e * <u, f> + g -> inf(u \in L_2[left, right) )
    def __init__(self, optimizer, left=0., right=1., a=1., b=2, c=1., e=1., g=42.):
        self.optimizer = optimizer
        self.optimizer.problem = self
        self.left = left
        self.right = right
        self.a = a
        self.b = b
        self.c = c
        self.e = e
        self.g = g

    def calc_f(self, x):
        return np.sin(x)

    def apply_linear_operator(self, u):
        '''
            return Du
            u is numpy array
        '''
        v = np.empty(10000)
        v[:1000] = u[9000:]
        v[1000:] = u[:9000]
        return 2 * u

    def apply_conjugate_operator(self, u):
        '''
            return D*(u)
            u is numpy array
        '''
        v = np.empty(10000)
        v[:9000] = u[1000:]
        v[:1000] = u[9000:]
        return 2 * u

    def calc_dot(self, f1, f2):
        '''
            return <u, f>
            u, f are numpy arrays
        '''
        return np.mean(f1 * f2)

    def calc_functional(self, u):
        # J(u) = a * ||u||^b + c * <Du, u> + e * <u, f> + g
        J = self.a * (self.calc_dot(u, u) ** (self.b / 2)) + self.c * self.calc_dot(self.apply_linear_operator(u), u) + \
            self.e * self.calc_dot(u, self.f) + self.g
        return J

    def calc_deriative(self, u):
        # J'(u) = a * b * ||u||^{b-2} * u + c * (D + D*)u + e * f
        deriv_J = self.a * self.b * (self.calc_dot(u, u) ** (self.b / 2 - 1)) * u + \
                  self.c * (self.apply_linear_operator(u) + self.apply_conjugate_operator(u)) + self.e * self.f
        return deriv_J

    def calc_hessian(self, u):
        # J''(u) = a * (b * (b - 2) * ||u||^{b-4} * <u, h> * u + b * ||u||^{b - 2} * I) + c * (D + D*)
        operator = u[np.newaxis, :] * u
        matrix_of_linear_operator = np.identity(u.size)
        matrix_of_conjugate_operator = np.identity(u.size)
        self.a * (self.b * (self.b - 2) * (self.calc_dot(u, u) ** (self.b / 2 - 2)) * operator + \
                  self.b * (self.calc_dot(u, u) ** (self.b / 2 - 1)) * np.identity(u.size)) + \
                  self.c * (matrix_of_linear_operator + matrix_of_conjugate_operator)
        pass

    def solve(self):
        self.f = self.calc_f(self.left + (self.right - self.left) / self.optimizer.num_of_points * \
                             np.arange(self.optimizer.num_of_points))
        return self.optimizer.optimize()


class LinearProgramming():
    def __init__(self, optimizer, A, b, c):
        # J(u) = <c, u> -> inf w.r.t Au = b, u >= 0
        self.optimizer = optimizer
        self.A = A
        self.b = b
        self.c = c
        self.optimizer.problem = self

    def solve(self):
        return self.optimizer.optimize()