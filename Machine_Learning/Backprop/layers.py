import numpy as np


class Layer:
    """
    A building block. Each layer is capable of performing two things:

    - Process input to get output:           output = layer.forward(input)

    - Propagate gradients through itself:    grad_input = layer.backward(input, grad_output)

    Some layers also have learnable parameters which they update during layer.backward.
    """

    def __init__(self):
        """
        Here you can initialize layer parameters (if any) and auxiliary stuff.
        """

        raise NotImplementedError("Not implemented in interface")

    def forward(self, input):
        """
        Takes input data of shape [batch, ...], returns output data [batch, ...]
        """

        raise NotImplementedError("Not implemented in interface")

    def backward(self, input, grad_output):
        """
        Performs a backpropagation step through the layer, with respect to the given input.
        Updates layer parameters and returns gradient for next layer
        Let x be layer weights, output – output of the layer on the given input and grad_output – gradient of layer with respect to output

        To compute loss gradients w.r.t parameters, you need to apply chain rule (backprop):
        (d loss / d x)  = (d loss / d output) * (d output / d x)
        Luckily, you already receive (d loss / d output) as grad_output, so you only need to multiply it by (d output / d x)
        If your layer has parameters (e.g. dense layer), you need to update them here using d loss / d x

        returns (d loss / d input) = (d loss / d output) * (d output / d input)
        """

        raise NotImplementedError("Not implemented in interface")


class ReLU(Layer):
    def __init__(self):
        """
        ReLU layer simply applies elementwise rectified linear unit to all inputs
        This layer does not have any parameters.
        """

    def forward(self, input):
        """
        Perform ReLU transformation
        input shape: [batch, input_units]
        output shape: [batch, input_units]
        """

        output = input
        output[output < 0] = 0
        return output

    def backward(self, input, grad_output):
        """
        Compute gradient of loss w.r.t. ReLU input
        """

        grad_output[input <= 0] = 0
        return grad_output


class Dense(Layer):
    def __init__(self, input_units, output_units, learning_rate=0.1):
        """
        A dense layer is a layer which performs a learned affine transformation:
        f(x) = Wx + b

        W: matrix of shape [num_inputs, num_outputs]
        b: vector of shape [num_outputs]
        """

        self.learning_rate = learning_rate

        # initialize weights with small random numbers from normal distribution
        # self.weights = np.random.normal(0, np.sqrt(2 / (input_units + output_units)), (input_units, output_units))
        self.weights = np.random.normal(0, np.sqrt(1 / input_units), (input_units, output_units))
        self.biases = np.zeros(output_units)

    def forward(self, input):
        """
        Perform an affine transformation:
        f(x) = <W*x> + b

        input shape: [batch, input_units]
        output shape: [batch, output units]
        """

        return np.dot(input, self.weights) + self.biases[np.newaxis, :]

    def backward(self, input, grad_output):
        """
        input shape: [batch, input_units]
        grad_output: [batch, output units]

        Returns: grad_input, gradient of output w.r.t input
        """

        # weights_grad: [batch_size, input_units, output_units]
        grad_prop = np.dot(grad_output, self.weights.T)
        self.weights -= self.learning_rate * np.dot(input.T, grad_output)
        self.biases -= self.learning_rate * grad_output.sum(axis=0)
        return grad_prop


class Conv2d(Layer):
    def __init__(self, in_channels, out_channels, kernel_size, learning_rate=0.1):
        """
        A convolutional layer with out_channels kernels of kernel_size.

        in_channels — number of input channels
        out_channels — number of convolutional filters
        kernel_size — tuple of two numbers: k_1 and k_2

        Initialize required weights.
        """

        self.learning_rate = learning_rate
        kernel_height = kernel_size[0]
        kernel_width = kernel_size[1]

        self.weights = np.random.normal(0, np.sqrt(2 / ((in_channels + out_channels) * kernel_width * kernel_height)),
                                       (in_channels, out_channels, kernel_height, kernel_width))

    def forward(self, input):
        """
        #Perform convolutional transformation:

        #input shape: [batch, in_channels, h_in, w_in]
        #output shape: [batch, out_channels, h_out, w_out]
        """

        h_in, w_in = input.shape[2:]
        kernel_height, kernel_width = self.weights.shape[2:]
        h_out = h_in - kernel_height + 1
        w_out = w_in - kernel_width + 1
        batch_size = input.shape[0]
        out_channels = self.weights.shape[1]
        output = np.empty((batch_size, out_channels, h_out, w_out))
        for row in range(h_out):
            for column in range(w_out):
                prod = input[:, :, np.newaxis, row:row + kernel_height, column:column + kernel_width] * \
                       self.weights[np.newaxis, :, :, :, :]
                output[:, :, row, column] = prod.sum(axis=(1, 3, 4))
        return output

    def backward(self, input, grad_output):
        """
        #Compute gradients w.r.t input and weights and update weights

        #input shape: [batch, in_channels, h_in, w_in]
        #grad_output shape: [batch, out_channels, h_out, w_out]
        """

        batch_size, in_channels, h_in, w_in = input.shape
        out_channels, h_out, w_out = grad_output.shape[1:]
        kernel_height, kernel_width = self.weights.shape[2:]
        prop_grad = np.zeros(input.shape, dtype=float)
        for row in range(h_out):
            for col in range(w_out):
                prop_grad[:, :, row:row + kernel_height, col:col + kernel_width] += \
                        (grad_output[:, :, row, col][:, np.newaxis, :, np.newaxis, np.newaxis] * \
                        self.weights[np.newaxis, :, :, :, :]).sum(axis=2)

        weights_update = np.empty(self.weights.shape)
        for row in range(kernel_height):
            for column in range(kernel_width):
                weights_update[:, :, row, column] = \
                        (input[:, :, np.newaxis, row:row + h_out, column:column + w_out] * \
                        grad_output[:, :, row, column][:, np.newaxis, :, np.newaxis, np.newaxis]).sum(axis=(0, 3, 4))
        self.weights -= self.learning_rate * weights_update
        return prop_grad


class Maxpool2d(Layer):
    def __init__(self, kernel_size):
        """
        A maxpooling layer with kernel of kernel_size.
        This layer donwsamples [kernel_size, kernel_size] to
        1 number which represents maximum.

        Stride description is identical to the convolution
        layer. But default value we use is kernel_size to
        reduce dim by kernel_size times.

        This layer does not have any learnable parameters.
        """

        self.stride = kernel_size
        self.kernel_size = kernel_size

    def forward(self, input):
        """
        Perform maxpooling transformation:

        input shape: [batch, in_channels, h, w]
        output shape: [batch, out_channels, h_out, w_out]
        """

        batch_size, out_channels, h_in, w_in = input.shape
        h_out = h_in // self.kernel_size
        w_out = w_in // self.kernel_size
        output = np.empty((batch_size, out_channels, h_out, w_out))
        for row in range(h_out):
            for col in range(w_out):
                output[:, :, row, col] = np.max(input[:, :, row * self.kernel_size:(row + 1) * self.kernel_size,
                                                            col * self.kernel_size:(col + 1) * self.kernel_size],
                                                            axis=(2, 3))
        return output

    def backward(self, input, grad_output):
        """
        Compute gradient of loss w.r.t. Maxpool2d input
        """

        batch_size, out_channels, h_in, w_in = input.shape
        in_channels, h_out, w_out = grad_output.shape[1:]
        grad_input = np.empty(input.shape)
        grad_output_coppied = np.empty(input.shape)
        for row in range(h_out):
            for col in range(w_out):
                max_val = np.max(input[:, :, row * self.kernel_size:(row + 1) * self.kernel_size,
                                             col * self.kernel_size:(col + 1) * self.kernel_size],
                                             axis=(2, 3))
                grad_input[:, :, row * self.kernel_size:(row + 1) * self.kernel_size,
                                 col * self.kernel_size:(col + 1) * self.kernel_size] = \
                                 max_val[:, :, np.newaxis, np.newaxis]
                grad_output_coppied[:, :, row * self.kernel_size:(row + 1) * self.kernel_size,
                                          col * self.kernel_size:(col + 1) * self.kernel_size] = \
                                          grad_output[:, :, row, col][:, :, np.newaxis, np.newaxis]
        mask = grad_input == input
        grad_input[mask] = grad_output_coppied[mask]
        grad_input[~mask] = 0
        return grad_input


class Flatten(Layer):
    def __init__(self):
        """
        This layer does not have any parameters
        """

    def forward(self, input):
        """
        input shape: [batch_size, channels, feature_nums_h, feature_nums_w]
        output shape: [batch_size, channels * feature_nums_h * feature_nums_w]
        """

        return input.reshape((input.shape[0], -1))

    def backward(self, input, grad_output):
        """
        Compute gradient of loss w.r.t. Flatten input
        """

        batch_size, channels, feature_nums_h, feature_nums_w = input.shape
        return grad_output.reshape((batch_size, channels, feature_nums_h, feature_nums_w))


def softmax(logits):
    softmax = np.exp(logits - logits.max(axis=1)[:, np.newaxis])
    softmax /= softmax.sum(axis=1)[:, np.newaxis]
    return softmax


def softmax_crossentropy_with_logits(logits, y_true):
    """
    Compute crossentropy from logits and ids of correct answers
    logits shape: [batch_size, num_classes]
    y_true: [batch_size]
    output is a number
    """

    softmax_val = softmax(logits)
    batch_size = softmax_val.shape[0]
    return -np.mean(np.log(softmax_val[np.arange(batch_size), y_true]))


def grad_softmax_crossentropy_with_logits(logits, y_true):
    """
    Compute crossentropy gradient from logits and ids of correct answers
    logits shape: [batch_size, num_classes]
    y_true: [batch_size]
    output: [batch_size, num_classes]
    """

    num_classes = logits.shape[1]
    batch_size = logits.shape[0]
    true_labels_map = (y_true[:, np.newaxis] == np.arange(num_classes)[np.newaxis, :]).astype(float)
    return -(true_labels_map - softmax(logits)) / batch_size