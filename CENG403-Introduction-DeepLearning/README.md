# CENG403 - Deep Learning Assignments (Spring 2024)

## Overview

This repository contains the solutions to three comprehensive homework assignments (THE1, THE2, and THE3) for the CENG403 course at Middle East Technical University (METU), Spring 2024. These assignments are designed to provide hands-on experience with deep learning concepts, including tensor operations, neural network implementation, convolutional neural networks (CNNs), and transfer learning.

## Homework Breakdown

### THE1: Custom Tensor Library Implementation

#### Objective:
The goal of THE1 was to create a custom tensor library, CerGen, from scratch. This library serves as the foundation for performing basic tensor operations similar to those found in popular deep learning frameworks like PyTorch and TensorFlow.

#### Key Features:

* Basic Arithmetic Operations: Implemented element-wise addition, subtraction, multiplication, and division for n-dimensional tensors.
* Mathematical Functions: Included operations like logarithms, sine, cosine, and exponentiation.
* Norm Calculations: Provided methods to calculate L1, L2, and Lp norms for tensors.
* Matrix Operations: Implemented matrix multiplication, transposition, and reshaping functionalities.
#### Outcome:
By completing THE1, a fully functional tensor library was created, which laid the groundwork for further deep learning tasks in subsequent assignments.

### THE2: Multi-Layer Perceptron (MLP) Training from Scratch
#### Objective:
THE2 extended the CerGen library by implementing and training a simple Multi-Layer Perceptron (MLP) classifier using only basic operations. This assignment focused on understanding and implementing forward propagation, backpropagation, and gradient descent without relying on high-level deep learning frameworks.

#### Key Features:

* Katman Class: Implemented linear layers (fully connected layers) with support for ReLU and Softmax activations.
* MLP Class: Built a basic MLP with one hidden layer and trained it on the MNIST dataset.
* Backpropagation: Extended the CerGen library to support gradient calculation and parameter updates using the chain rule.
* Training Process: Developed a custom training loop to train the MLP on a subset of the MNIST dataset.
#### Outcome:
THE2 provided a deeper understanding of how neural networks function at a fundamental level, including how to implement and train them from scratch.

### THE3: Convolutional Neural Networks and Transfer Learning
#### Objective:
THE3 involved implementing key operations for Convolutional Neural Networks (CNNs) from scratch, training CNNs using PyTorch, and fine-tuning a pre-trained ResNet18 model on the CIFAR10 dataset.

#### Key Features:

* Naive Convolution and Pooling: Implemented convolution and max-pooling operations without using matrix-vector multiplications.
* PyTorch CNN: Trained a small CNN on the CIFAR10 dataset using PyTorch, achieving moderate accuracy.
* Transfer Learning with ResNet18: Fine-tuned a pre-trained ResNet18 model for CIFAR10 classification, demonstrating the power of transfer learning.
#### Outcome:
THE3 provided hands-on experience with CNNs and transfer learning, highlighting how pre-trained models can be adapted to new tasks with minimal training.


## Conclusion

These assignments provided a comprehensive journey through key concepts in deep learning, from the basics of tensor operations to the intricacies of neural networks and convolutional networks. By completing these assignments, a strong foundation was built in both theoretical understanding and practical implementation of deep learning models.
