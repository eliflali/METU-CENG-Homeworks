# Gergen: A Python Library for Tensor Operations

## Introduction

Welcome to the Gergen library, a Python-based framework designed to perform tensor operations and calculations. This library is built for educational purposes and provides an alternative to popular libraries like NumPy, though with a simpler and more explicit implementation.

The library is developed to aid in understanding how tensor operations can be implemented from scratch. Gergen supports various tensor operations including element-wise operations, tensor reshaping, and common mathematical operations.

## Features

* Tensor Creation: Create tensors with specified dimensions and random values, either integers or floating-point numbers.
* Element-wise Operations: Perform addition, subtraction, multiplication, and division on tensors.
* Mathematical Functions: Calculate trigonometric functions (sin, cos, tan), logarithms, and powers for tensor elements.
* Norms Calculation: Calculate L1, L2, and Lp norms of tensors.
* Matrix Operations: Perform inner and outer products, transpose, and reshaping of tensors.
* Tensor Aggregation: Sum and average tensor elements along specified axes.

## Usage

### Creating Tensors
You can create random tensors with the rastgele_gercek or rastgele_dogal functions:

```python
boyut = (3, 3)
tensor = rastgele_gercek(boyut, aralik=(0.0, 1.0))
```

### Basic Operations
#### Gergen supports basic element-wise operations:


```python
A = rastgele_gercek((3, 3))
B = rastgele_gercek((3, 3))

# Addition
C = A + B

# Subtraction
D = A - B

# Multiplication
E = A * B

# Division
F = A / B

```

### Mathematical Functions
#### You can perform various mathematical operations on tensors:

``` python

# Trigonometric functions
sin_tensor = A.sin()
cos_tensor = B.cos()

# Logarithmic functions
log_tensor = A.log()  # Base-10 log
ln_tensor = B.ln()    # Natural log

# Power
squared_tensor = A.us(2)  # Square each element

```

### Norm Calculations
#### Calculate the L1, L2, or Lp norm of a tensor:

``` python
L1_norm = A.L1()
L2_norm = B.L2()
Lp_norm = A.Lp(3)  # L3 norm
```

### Matrix Operations
#### Perform matrix-specific operations like transposition or dot product:

```python
# Transpose
transposed_A = A.devrik()

# Inner product
inner_product = A.ic_carpim(B)

# Outer product
outer_product = A.dis_carpim(B)

```
### Aggregation Functions
#### You can also compute aggregated values along specific axes:

``` python
# Sum all elements
sum_all = A.topla()

# Sum along axis 0
sum_axis_0 = A.topla(eksen=0)

# Average all elements
average_all = A.ortalama()

# Average along axis 1
average_axis_1 = A.ortalama(eksen=1)
```
