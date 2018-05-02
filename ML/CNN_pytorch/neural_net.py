import torch
from torch.autograd import Variable
import torch.nn as nn
import torch.nn.functional as F


class ConvAutoEncoder(nn.Module):
    def __init__(self, conv_out_channels=40, 
                 conv_kernel_size=2, conv_stride=2, conv_padding=0,
                 pool_kernel_size=2, pool_stride=2, pool_padding=0):
        super(ConvAutoEncoder, self).__init__()
        # Define encoder layers
        self.conv = nn.Conv2d(kernel_size=conv_kernel_size, stride=conv_stride, padding=conv_padding,
                              in_channels=3, out_channels=conv_out_channels)
        self.pool = nn.MaxPool2d(kernel_size=pool_kernel_size, stride=pool_stride, padding=pool_padding, return_indices=True)
        # Define decoder layers
        self.unpool = nn.MaxUnpool2d(kernel_size=pool_kernel_size, stride=pool_stride, padding=pool_padding)
        self.deconv = nn.ConvTranspose2d(kernel_size=conv_kernel_size, stride=conv_stride, padding=conv_padding,
                                         in_channels=conv_out_channels, out_channels=3)

    def forward(self, x):
        input_size = x.size()
        x, pool_indices = self.pool(F.relu(self.conv(x)))
        self.inter_layer_size = x.size()
        # x = F.tanh(self.deconv(self.unpool(x, indices=pool_indices), output_size=input_size))
        x = self.deconv(F.relu(self.unpool(x, indices=pool_indices)), output_size=input_size)
        return x


class Encoder(nn.Module):
    def __init__(self, net):
        super(Encoder, self).__init__()
        self.conv = net.conv
        self.pool = net.pool

    def forward(self, x):
        x, _ = self.pool(F.relu(self.conv(x)))
        return x.view(-1, self.num_flat_features(x))

    def num_flat_features(self, x):
        size = x.size()[1:]  # all dimensions except the batch dimension
        num_features = 1
        for s in size:
            num_features *= s
        return num_features


class ConvNet(nn.Module):
    def __init__(self, num_of_conv_layers=1, num_of_fully_connected_layers=2,
                 conv_out_channels=(40, 20, 10), conv_kernel_size=(2, 2, 2), conv_stride=(2, 1, 1),
                 pool_kernel_size=(2, 2, 2), pool_stride=(2, 1, 1), linear_layer_input=2560):
        super(ConvNet, self).__init__()
        # Conv layers
        self.encoder = nn.Sequential(
            nn.Conv2d(in_channels=3,
                      out_channels=conv_out_channels[0],
                      kernel_size=conv_kernel_size[0],
                      stride=conv_stride[0]),
            nn.Tanh(),
            nn.MaxPool2d(kernel_size=pool_kernel_size[0], stride=pool_stride[0])
        )
        for i in range(1, num_of_conv_layers):
            self.encoder.add_module('conv' + str(i), nn.Conv2d(in_channels=conv_out_channels[i - 1],
                                                               out_channels=conv_out_channels[i],
                                                               kernel_size=conv_kernel_size[i],
                                                               stride=conv_stride[i]))
            self.encoder.add_module('act' + str(i), nn.Tanh())
            self.encoder.add_module('pool' + str(i), nn.MaxPool2d(kernel_size=pool_kernel_size[i],
                                                                  stride=pool_stride[i]))
        # Linear layers
        self.linear = nn.Sequential(
            nn.Linear(linear_layer_input, 64),
            nn.Tanh(),
            nn.Linear(64, 10)
        )

    def forward(self, x):
        x = self.encoder(x)
        x = x.view(-1, self.num_flat_features(x))
        return self.linear(x)

    def num_flat_features(self, x):
        size = x.size()[1:]  # all dimensions except the batch dimension
        num_features = 1
        for s in size:
            num_features *= s
        return num_features


class LinearNet(nn.Module):
    def __init__(self, input_size=3 * 32 * 32):
        self.input_size = input_size
        super(LinearNet, self).__init__()
        self.fc1 = nn.Linear(input_size, 10)

    def forward(self, x):
        x = x.view(-1, self.input_size)
        x = self.fc1(x)
        return x
