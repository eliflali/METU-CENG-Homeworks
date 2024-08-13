# THE3: Convolutional Neural Networks and Transfer Learning

## Overview

This repository contains the implementation of a series of tasks related to Convolutional Neural Networks (CNNs) and transfer learning as part of THE3 for the CENG403 course at METU, Spring 2024. The tasks are designed to provide hands-on experience in implementing CNN operations from scratch, training CNNs using PyTorch, and fine-tuning a pre-trained ResNet18 model on the CIFAR10 dataset.

## Tasks

* ### Task 1: Naive Convolution and Pooling Implementation
In this task, we implemented convolution and max-pooling operations from scratch without using any matrix-vector multiplications. The implementation is purely based on nested loops to perform these operations.

#### Key components:

* Convolution Forward Pass: Implemented the 2D convolution operation using nested loops. The function computes the convolution of the input with multiple filters.
* Convolution Backward Pass: Implemented the backward pass for the convolution operation to calculate gradients with respect to inputs, filters, and biases.
* Max-Pooling Forward Pass: Implemented the forward pass for max-pooling, which reduces the spatial dimensions of the input.
* Max-Pooling Backward Pass: Implemented the backward pass for max-pooling to propagate gradients back through the network.

* ### Task 2: Implementing a CNN with PyTorch
In this task, we created and trained a CNN using PyTorch. The model was trained on the CIFAR10 dataset, a common benchmark dataset for image classification tasks.

#### Key components:

* Model Definition: Created a three-layer CNN with convolutional layers followed by fully connected layers.
* Training the Model: Implemented a training loop to train the CNN on the CIFAR10 dataset using stochastic gradient descent (SGD) as the optimizer.
* Evaluation: Analyzed the model's performance by visualizing the loss curve and calculating accuracy on the test set.

* ### Task 3: Finetuning ResNet18
This task involved transfer learning, where we fine-tuned a pre-trained ResNet18 model on the CIFAR10 dataset. The pre-trained model was adapted to the new dataset by replacing its final fully connected layer.

#### Key components:

* Downloading and Adapting ResNet18: Loaded a pre-trained ResNet18 model and replaced its final layer to output predictions for CIFAR10 classes.
* Freezing Parameters: Froze the pre-trained layers of ResNet18 to retain the learned features while only training the new fully connected layer.
* Training the Fine-tuned Model: Trained the adapted ResNet18 model on CIFAR10 using the previously defined training loop.
* Evaluation: Assessed the model's performance on CIFAR10 by calculating accuracy and visualizing the model's predictions.

  
## Structure
* Task 1: Implementation of naive convolution and pooling operations.
* Task 2: Training a small CNN on CIFAR10 using PyTorch.
* Task 3: Fine-tuning ResNet18 for CIFAR10 classification.
  
## Results

### Task 1
Successfully implemented convolution and max-pooling operations using nested loops.
Verified the correctness of the forward and backward passes by comparing them with expected outputs.
### Task 2
Trained a small CNN achieving around 55% accuracy on CIFAR10.
Visualized the loss curve showing the training progress over 10 epochs.
### Task 3
Fine-tuned a ResNet18 model to achieve 69% accuracy on CIFAR10 with just two epochs of training on the final layer.
Demonstrated the effectiveness of transfer learning by adapting a pre-trained model to a new dataset.


## Conclusion

This project provided practical experience in both implementing core CNN operations from scratch and leveraging modern deep learning frameworks for model training and transfer learning. The tasks helped in understanding the fundamentals of CNNs, as well as how to apply and fine-tune pre-trained models for new tasks.
