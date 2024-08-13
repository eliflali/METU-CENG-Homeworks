# CerGen: Custom Tensor Library and MLP Implementation

## Overview

This repository contains the implementation of a custom tensor library, CerGen, and a simple Multi-Layer Perceptron (MLP) classifier. The project was developed as part of a take-home exam for the CENG 403 course at Middle East Technical University (METU), Spring 2024.

The primary objective of this project was to extend the functionality of the CerGen tensor library (initially developed in a previous homework) to train a basic MLP on the MNIST dataset. This project involved implementing forward and backward passes, activation functions, and gradient descent from scratch, without relying on high-level deep learning frameworks such as TensorFlow, PyTorch, or NumPy.

## Project Structure

* CerGen Tensor Library: A custom library designed to perform tensor operations similar to what is available in frameworks like PyTorch and TensorFlow.
* MLP Implementation: A simple feedforward neural network trained on the MNIST dataset using the CerGen library.

### Key Features

1. CerGen Tensor Library
The CerGen library provides essential functionalities to create and manipulate tensors:

* Tensor Creation: Generate tensors with random values using specified dimensions.
* Element-wise Operations: Support for basic operations like addition, subtraction, multiplication, and division.
* Mathematical Functions: Implementations of common functions such as logarithms, trigonometric operations, and power functions.
* Norms: Calculation of L1, L2, and Lp norms.
* Matrix Operations: Includes matrix multiplication, transposition, and reshaping functionalities.
* Forward and Backward Passes: Support for implementing neural networks and training them using backpropagation.
  
2. Multi-Layer Perceptron (MLP)
The MLP implementation was designed with the following components:

* Katman Class: Represents a single linear layer in the neural network. It initializes the layer's weights and biases, performs the forward pass, and applies activation functions.
* MLP Class: Represents the complete MLP with one hidden layer and an output layer. The forward pass involves matrix multiplication, adding biases, applying activation functions (ReLU and Softmax), and producing the final output.
* Activation Functions: Includes ReLU and Softmax, implemented as part of the CerGen library to introduce non-linearity and convert outputs to probabilities.
* Loss Function: Cross-Entropy Loss is implemented to evaluate the error between predicted and true labels.
* Backpropagation and Gradient Descent: The backward pass was implemented to update the network's weights and biases based on the gradients computed during backpropagation.

## Detailed Implementation

### Revisiting THE1
In the first part of the project (THE1), several crucial tensor operations were implemented:

* Basic Arithmetic Operations: Addition, subtraction, multiplication, and division of tensors.
* Norm Calculations: Implementing L1, L2, and Lp norms for tensors.
* Forward Pass: Initial implementation of the ileri() function for simple forward passes in neural networks.

### Extending the Tensor Library
For this project (THE2), the CerGen library was extended to support neural network training:

* Katman Class: Represents the building block of neural networks, allowing for linear transformations of inputs using weight matrices and biases.
* MLP Class: Implements a simple MLP with one hidden layer and an output layer. The forward pass was implemented to propagate inputs through the network.
* Backpropagation (geri()): Implemented the geri() function in the Operation class, allowing for the calculation of gradients during the backward pass.
* Gradient Calculation (turev al()): Extended the Gergen class to track operations and compute gradients using the chain rule, essential for updating network parameters during training.

### Training the MLP
The egit() function was implemented to train the MLP on the MNIST dataset:

* Input Data: Handled as a list of input tensors and target tensors, representing images and labels.
* Forward Pass: Input tensors were propagated through the network to generate predictions.
* Loss Calculation: Cross-Entropy Loss was computed between predictions and true labels.
* Backward Pass: Gradients were computed and used to update the network's weights and biases.
* Parameter Tuning: Hyperparameters such as learning rate and hidden layer size were tuned to optimize the model.
* Evaluation and Performance
* Performance Comparison: Although the custom MLP implementation is not as optimized as frameworks like PyTorch, it provides a hands-on understanding of how neural networks work at a fundamental level.
* Loss Curves: Plotted loss curves for different hyperparameter settings to find the most effective model configuration.

### PyTorch Implementation
In addition to the custom MLP, a similar MLP model was implemented using PyTorch to compare the results. The PyTorch implementation served as a benchmark to evaluate the correctness of the custom implementation.
