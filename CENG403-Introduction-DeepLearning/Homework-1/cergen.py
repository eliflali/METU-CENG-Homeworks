import random
import math
from typing import Union
import numpy as np
import time




def cekirdek(sayi: int):
    #Sets the seed for random number generation
    return random.seed(sayi)


def generate_gergen(boyut, generator, level=0):
    # Generate the tensor data recursively
    if level == len(boyut) - 1:
        return gergen([generator() for _ in range(boyut[level])]) 
    else:
        return gergen([generate_gergen(boyut, generator, level + 1).veri() for _ in range(boyut[level])])
       

def rastgele_dogal(boyut, aralik=(0,100), dagilim='uniform'):
    """
    Generates data of specified dimensions with random integer values and returns a gergen object.

    Parameters:
    boyut (tuple): Shape of the desired data.
    aralik (tuple, optional): (min, max) specifying the range of random values. Defaults to (0,100), which implies a default range.
    dagilim (string, optional): Distribution of random values ('uniform'). Defaults to 'uniform'.

    Returns:
    gergen: A new gergen object with random integer values.
    """
    if dagilim == 'uniform': #only uniform
        generator = lambda: random.randint(aralik[0], aralik[1])
    else:
        raise ValueError("Unsupported distribution type. Please use 'uniform'.")
    
    return generate_gergen(boyut, generator)

@staticmethod
def rastgele_gercek(boyut, aralik=(0.0, 1.0), dagilim='uniform'):
    """
    Generates a gergen of specified dimensions with random floating-point values.

    Parameters:
    boyut (tuple): Shape of the desired gergen.
    aralik (tuple, optional): (min, max) specifying the range of random values. Defaults to (0.0, 1.0) for uniform distribution.
    dagilim (string, optional): Distribution of random value ('uniform'). Defaults to 'uniform'.

    Returns:
    gergen: A new gergen object with random floating-point values.
    """

    if dagilim == 'uniform':
        generator = lambda: random.uniform(aralik[0], aralik[1])

    else:
        raise ValueError("Unsupported distribution type. Please use 'uniform'.")
    
    gergen_generated = generate_gergen(boyut, generator)
    #("a", gergen_generated)
    return gergen_generated



class gergen:

    __veri = None # int/float, list, list of lists
    D = None # Transpose of data
    __boyut = None #Dimensions of the derivative (Shape)


    def __init__(self, veri=None):
    # The constructor for the 'gergen' class.
    #
    # This method initializes a new instance of a gergen object. The gergen can be
    # initialized with data if provided; otherwise, it defaults to None, representing
    # an empty tensor.
    #
    # Parameters:
    # veri (int/float, list, list of lists, optional): A nested list of numbers that represents the
    # gergen data. The outer list contains rows, and each inner list contains the
    # elements of each row. If 'veri' is None, the tensor is initialized without data.
    #
    # Example:
    # To create a tensor with data, pass a nested list:
    # tensor = gergen([[1, 2, 3], [4, 5, 6]])
    #
    # To create an empty tensor, simply instantiate the class without arguments:
    # empty_tensor = gergen()
        self.__veri = veri
        #self.__boyut = self._compute_boyut(self.__veri)
        
    
    def veri(self):
        return self.__veri

    def _compute_boyut(self, veri):
        if isinstance(veri, (int, float)): #may be a scalar -- not sure how to represent
            return ()
        elif isinstance(veri, list): 
            # if it is a list
            # trace all dimensions recursively
            if(len(veri)>0):
                if isinstance(veri[0], (int,float)):
                    return (len(veri),)
                else:   
                    return (len(veri),) + self._compute_boyut(veri[0])
            else:
                return (0,)
        else:
            return (0,)
     
    def get_item_helper(self, data, index):
        if isinstance(data, list):
            if isinstance(index, list):
                return self.get_item_helper(data[index[0]],  index[1:])
            else:
                return data[index]
        else:
            return data
                 
    def __getitem__(self, index) -> 'gergen':
        veri = self.veri()
        indexed_item = self.get_item_helper(veri, index)
        return gergen(indexed_item) #returns gergen


    def traversal_print(self, tensor):
        """
        Recursively formats a tensor (nested list) with a given precision.
        Adjusts indentation based on the depth of the tensor to simulate NumPy-style formatting.
        """
        def format_row(row):
            return "[" + ",".join(str(n) for n in row) + "]"
    
        def format_element(e, depth, boyut, current_boyut):
            # Base case: the element is a number.
            if isinstance(e, list):
                if(len(e)<=0):
                    return "[]"
                if isinstance(e[0], (float, int)):
                    return format_row(e)
                else:
                    inner = '\n'.join([format_element(x, depth+1, boyut, current_boyut) for x in e])
                    depth = depth - 1
                    if depth == 0:
                        return '[' + inner + ']'
                    else:
                        return '[' + inner + ']'
        return format_element(tensor, 0, self.boyut, 0)
    
    def __str__(self):
        """
        2x3 boyutlu gergen : 
        [[1, 2, 3]
        [4, 5, 6]]
        """
        if isinstance(self.veri(), (int, float)):  # if scalar
            boyut_str = '0 boyutlu gergen:\n'
            return boyut_str + f"{self.veri()}"

        # boyut string
        dim = self.boyut()
        boyut_str = 'x'.join(map(str, dim)) + ' boyutlu gergen:\n'
        
        matrix_str = self.traversal_print(self.veri())
        return boyut_str + matrix_str

    # private method -- helper function
    # one operationi two parameters
    def _element_wise_operation(self, other, operation, zero_check=False, veri = None): # 2 params operations
        
        # type check done in every func in beginning, start tracing matrices.
        if veri == None:
            veri = self.veri()
        if isinstance(other, gergen):
            # recursively trace tensors:
            
            #dimensions of two gergen instances do not align for element-wise multiplication
            if self.boyut() != other.boyut():
                raise ValueError("Dimension does not match")
                
            # zip to make operations -- for loop
            return [self._element_wise_operation(subother, operation, zero_check, subself)
                    for subself, subother in zip(veri, other.veri())]
        elif isinstance(other, list):
            return [self._element_wise_operation(subother, operation, zero_check, subself)
                    for subself, subother in zip(veri, other)]
        
        else:
            # base -- zero check needed for division!
            if zero_check and other == 0: # if division
                raise ZeroDivisionError("Division by zero is undefined")
            
            result = self._apply_to_elements(operation, veri, other)
            return result # operation -> lambda func given by caller

    # private method -- helper function
    # one operation, one parameter
    # operation is what to apply
    # data is 
    def _apply_to_elements(self, operation, veri = None, operand = None): 
        # subdata -> recursively traces
        # if still is a list (not a scalar) keep recursion continuing
        if veri == None:
            veri = self.__veri
        if isinstance(veri, list): 
            return [self._apply_to_elements(operation, subveri, operand) for subveri in veri]
            
        # base condition
        else:
            if operand == None:
                return operation(veri)
            else:
                return operation(veri, operand)

    # private method -- helper function
    # returns scalar, applying one operation to all elements
    # initial value may change for multiplication -- as a parameter caller gives
    def _apply_operation_all_elmts(self, operation, initial_value, tensor = None):
        if tensor == None:
            tensor = self.__veri
            
        result = initial_value

        # recursively apply to all elements.
        # if type is gergen, take veri inside of gergen.
        if isinstance(tensor, gergen):
            tensor = self.__veri
        if isinstance(tensor, list):
            for element in tensor:
                result = self._apply_operation_all_elmts(operation, result,element)

        # base case -> operand (tensor) is a scalar
        else:
            result = operation(result, tensor)
    
        return result
            
    def __mul__(self, other: Union['gergen', int, float]) -> 'gergen':
        """
        Multiplication operation for gergen objects.
        Called when a gergen object is multiplied by another, using the '*' operator.
        Could be element-wise multiplication or scalar multiplication, depending on the context.
        """
        # an incompatible type is provided
        if not isinstance(other, (int, float, gergen)):
            raise TypeError("Unsupported type for division")
            
        # no zero check needed
        # lambda: f(x,y) -> x * y ; x is self
        multiplied_gergen = self._element_wise_operation(other.veri(), lambda x, y: x * y, zero_check = False) 
        return gergen(multiplied_gergen) # make return type gergen

    #CHECK THIS FUNCTION FOR EDGE CASES!!!!
    def __truediv__(self, other: Union['gergen', int, float]) -> 'gergen':
        """
        Division operation for gergen objects.
        Called when a gergen object is divided by another, using the '/' operator.
        The operation is element-wise.
        """
        # an incompatible type is provided
        if not isinstance(other, (int, float, gergen)):
            raise TypeError("Unsupported type for division")
        
        # Perform element-wise division recursively with zero division check
        # argument 3 is func to be executed
        # lambda: f(x,y) -> x / y ; x is self
        operation = lambda x, y: x / y
        divided_gergen = gergen(self._element_wise_operation(other, operation, zero_check=True)) 
        return divided_gergen



    def __add__(self, other: Union['gergen', int, float]) -> 'gergen':
        """
        Defines the addition operation for gergen objects.
        Called when a gergen object is added to another, using the '+' operator.
        The operation is element-wise.
        """
        # an incompatible type is provided
        if not isinstance(other, (int, float, gergen)):
            raise TypeError("Unsupported type for division")

        # lambda: f(x,y) -> x + y ; x is self
        # no zero check needed
        # return a gergen
        summation = lambda x, y: x + y
        added_gergen = gergen(self._element_wise_operation(other, summation, zero_check=False)) 
        return added_gergen

    def __sub__(self, other: Union['gergen', int, float]) -> 'gergen':
        """
        Subtraction operation for gergen objects.
        Called when a gergen object is subtracted from another, using the '-' operator.
        The operation is element-wise.
        """
        # an incompatible type is provided
        if not isinstance(other, (int, float, gergen)):
            raise TypeError("Unsupported type for division")

        # lambda: f(x,y) -> x - y ; x is self
        # no zero check needed
        # return a gergen
        
        subtracted_gergen = gergen(self._element_wise_operation(other.veri(), lambda x, y: x - y, zero_check=False)) 
        return subtracted_gergen


    def uzunluk(self):
        # Returns the total number of elements in the gergen
        # previously calculated boyut: (x,y,z,...) -- multiplication needed
        uzunluk = 1
        # calculate boyut if not already calculated:
        # boyut = self.__boyut ? self.__boyut : self.__calculate_boyut(self.__veri)
        dim = None
        if self.__boyut:
            dim = self.__boyut
        else:
           dim = self.boyut()
            
        # loop in elements of boyut tuple
        for dimension in dim:
            uzunluk  = uzunluk*dimension

        return uzunluk

    def boyut(self):
        # Returns the shape of the gergen
        # calculate boyut if not already calculated:
        dimension = None
        if self.__boyut:
            dimension = self.__boyut
        else:
           dimension = self._compute_boyut(self.__veri) 
        return dimension

    def combination(self, comb_list): #comb_list consists of 2 lists
        result = []
        for a in comb_list[0]:
            for b in comb_list[1]:
                result.append([a,b])
        return result
            
    def assign_element(self, data, index, value):
        if isinstance(data, list):
            if isinstance(index, list):
                if len(index) == 1:
                    data[index[0]] = value
                else:
                    if isinstance(data[0], list):
                        return self.assign_element(data[index[0]],  index[1:], value)
                    else:
                        data[index[0]] = value
                    
            else:
                data[index] = value
        else:
            self.__veri = value
        
        return value
            
        
    def devir(self, dim = []):
        dim = self.boyut()
        new_dim = []
        for i in dim:
            new_dim.append(i)
        new_dim.reverse()
        
        dim_list = []
        for i in dim:
            dim_list.append([a for a in range(0,i)])
        
        for a in range(len(dim_list)-1):
            dim_list[a+1] = self.combination(dim_list[a:a+2])
        
        dim_combinations = dim_list[-1]
        flat_combinations = []
        for i in range(len(dim_combinations)):
            a = self._traverse_elmts([], dim_combinations[i])
            flat_combinations.append(a) # consist of all combinations of dimensions
            
        
        transposed_gergen = rastgele_gercek(new_dim)
        
        transposed_veri = transposed_gergen.listeye()
        
        for i in flat_combinations:
            trans_coord = []
            for a in i:
                trans_coord.append(a)
            trans_coord.reverse()
            value = self.__getitem__(i)
            self.assign_element(transposed_veri, trans_coord, value.veri())

        return gergen(transposed_veri)
            
            
    def devrik(self): # must return gergen object
        # Returns the transpose of gergen
        # write helper to calculate transpose.
        dim = self.boyut()
        dim_list = []
        for i in dim:
            dim_list.append(i)
        #dim_list.reverse()
        #devrik_gergen = self.transpose(self.veri(), dim_list)

        devrik_gergen = self.devir(dim)
        self.D = devrik_gergen
        return devrik_gergen

    def sin(self):
        #Calculates the sine of each element in the given `gergen`.
        # must return a gergen
        # need to write _apply_to_elements!! don't forget
        operation = math.sin
        sin_gergen = gergen(self._apply_to_elements(operation))
        return sin_gergen

    def cos(self):
        #Calculates the cosine of each element in the given `gergen`.
        operation = math.cos
        cos_gergen = gergen(self._apply_to_elements(operation))
        return cos_gergen

    def tan(self):
        #Calculates the tangent of each element in the given `gergen`.
        tan_gergen = gergen(self._apply_to_elements(math.tan))
        return tan_gergen

    def us(self, n: int):
        # Raises each element of the gergen object to the power 'n'. This is an element-wise operation.
        # If n is negative, an error will be raised to indicate n should be an integer.
        if n < 0:
            raise ValueError("n should be positive") # not sure if it should be an int?

        operation = lambda x: math.pow(x, n)
        us_gergen = gergen(self._apply_to_elements(operation))
        return us_gergen

    def log(self):
        #Applies the logarithm function to each element of the gergen object, using the base 10.
        operation = math.log10
        log10_gergen = gergen(self._apply_to_elements(operation))
        return log10_gergen

    def ln(self):
        #Applies the natural logarithm function to each element of the gergen object.
        operation = math.log
        log_gergen = gergen(self._apply_to_elements(operation))
        return log_gergen

    def L1(self):
        # Calculates and returns the L1 norm
        # 2 steps:
        # 1 - take absoulute value of all elements in a tensor
        # 2 - sum all elements of abs tensor

        #STEP 1:
        operation = abs #?
        absolute_valued_gergen = gergen(self._apply_to_elements(operation))

        #STEP 2:
        summation = lambda x, y: x + y
        L1_norm = self._apply_operation_all_elmts(summation,0,absolute_valued_gergen)
        
        return L1_norm
        

    def L2(self):
        # Calculates and returns the L2 norm
        # 3 steps:
        # 1 - take squares of all elements in a tensor
        # 2 - sum all elements of squared tensor
        # 3 - take square root of sum

        # STEP 1:
        squared_gergen = self.us(2) # using func us I defined above

        # STEP 2:
        summation = lambda x, y: x + y
        # for summation initial_value must be 0
        summation_result = self._apply_operation_all_elmts(summation,0, squared_gergen)

        # STEP 3:
        L2_norm = math.sqrt(summation_result)
        
        return L2_norm

    def Lp(self, p):
        # Calculates and returns the Lp norm, where p should be positive integer
        # 4 steps:
        # 0 - check if p<0
        # 1 - take p^th values of all elements in a tensor
        # 2 - sum all elements of squared tensor
        # 3 - take p^th root of sum
        #STEP 0:
        if p<0:
            # already would be raised from us
            # written for clean coding purposes
            raise ValueError("p should be positive") 
        
        # STEP 1:
        pth_gergen = self.us(p) # using func us I defined above

        # STEP 2:
        summation = lambda x, y: x + y
        # for summation initial_value must be 0
        summation_result = self._apply_operation_all_elmts(summation,0, pth_gergen)

        # STEP 3:
        Lp_norm = summation_result**1/p
        
        return Lp_norm

    def listeye(self):
        #Converts the gergen object into a list or a nested list, depending on its dimensions.
        # what to do in it?
        return self.veri()

    def _traverse_elmts(self, result_list, subself=None):
        if subself == None:
            subself = self.veri()

        # recursively apply to all elements.
        # if type is gergen, take veri inside of gergen.
        if isinstance(subself, gergen):
            subself = self.veri()
            
        if isinstance(subself, list):
            for element in subself:
                self._traverse_elmts(result_list, element)

        # base case -> operand (tensor) is a scalar
        else:
            result_list.append(subself)
    
        return result_list
    
    def duzlestir(self): #-> gergen
        #Converts the gergen object's multi-dimensional structure into a 1D structure, effectively 'flattening' the object.
        duz_gergen = self._traverse_elmts([])
        return gergen(duz_gergen)

    def boyutlandir(self, yeni_boyut):
        #Reshapes the gergen object to a new shape 'yeni_boyut', which is specified as a tuple.
        flattened_veri = self.duzlestir()
        boyutlandirilmis_gergen = self.format_matrix(flattened_veri, yeni_boyut)
        return boyutlandirilmis_gergen
    
    def ic_carpim(self, other): # -> (float,gergen)
        #Calculates the inner (dot) product of this gergen object with another.
        if not (isinstance(self, gergen) or isinstance(other, gergen)):
            raise TypeError("Both operands must be gergen instances.")
        self_dim = self.boyut()
        other_dim = other.boyut()
        if (len(self_dim) == 1 and len(other_dim) == 1):
            if self_dim[0] != other_dim[0]:
                raise ValueError("Equal dimensionality is required for inner product computation.")
            else:
                return sum(x*y for x, y in zip(self.veri(), other.veri()))
            
        elif (len(self_dim) == 2 and len(other_dim) == 2):
            if self_dim[1] != other_dim[0]:
                raise ValueError("Compatible dimensions in matrix multiplication is required.")
            else:
                result = [[sum(a*b for a, b in zip(X_row, Y_col)) for Y_col in zip(*other.veri())] for X_row in self.veri()]
                return gergen(result)
            
        else:
            raise ValueError("Unsupported gergen boyut for inner product.")
            

    def dis_carpim(self, other):
        if not isinstance(other, gergen):
            raise TypeError("Both operands must be gergen instances.")
        self_boyut = self.boyut()
        other_boyut = other.boyut()
        if not (len(self_boyut) == 1 or len(other_boyut) ==1):
            raise ValueError("Both operands must be 1-D arrays to compute the outer product.")
        self_veri = self.veri()
        other_veri = other.veri()
        result = []
        for s in self_veri:
            row = []
            for o in other_veri:
                row.append(s*o)
            result.append(row)
        result = gergen(result)._traverse_elmts([], result)
        result = self.format_matrix(result, [other_boyut[0], self_boyut[0]])
        return result
                
                
        
        
    
    # helper function for topla
    # takes a flattened gergen and a custom boyut
    # reshapes matrix accordingly
    # returns list
    # important that gergen should be flattened beforehand with func -> duzlestir
    def format_matrix(self, duz_tensor, boyut):
        if isinstance(duz_tensor, gergen): # in case duz_tensor is a gergen
            duz_tensor = duz_tensor.veri()
            
        if len(boyut) == 1:  # base
            return duz_tensor
        
        size = boyut[0]
        sublist_size = int(len(duz_tensor) / size)
        return [self.format_matrix(duz_tensor[i*sublist_size:(i+1)*sublist_size], boyut[1:])
                for i in range(size)]
            
    # helper function for topla
    def sum_eksen(self, initial_value,eksen,tensor = None):
        if tensor == None:
            tensor = self.__veri
            
        result = initial_value

        if eksen>0:
            for element in tensor:
                result = self.sum_eksen(result,eksen-1, element)
        else:
            single_element = []
            for element in tensor:
                single_element.append(element)
                
            single_result = 0
            summation = lambda x, y: x + y
            for element in single_element:
                single_result = self._element_wise_operation(single_result, summation, False, element)
            result.append(single_result)
        return result
    
    def topla(self, eksen=None):
        #Sums up the elements of the gergen object, optionally along a specified axis 'eksen'.
        if eksen is not None and not isinstance(eksen, int):
            raise TypeError("Eksen must be an integer or None.")
        if eksen ==  None:
            summation = lambda x, y: x + y
            result = self._apply_operation_all_elmts(summation,0)
            
        else:
            if eksen>len(self.boyut()):
                raise ValueError("Specified eksen is out of bounds")
            toplam = self.sum_eksen([], eksen)
            new_dim = []
            dim = self.boyut()
            for i in range(0, len(dim)):
                if i == eksen:
                    continue
                else:
                    new_dim.append(dim[i])
            toplam = gergen(toplam).duzlestir()
            result = self.format_matrix(toplam.veri(), new_dim)
                
                
        return gergen(result)
            
    # helper function for ortalama
    def avg_eksen(self, initial_value,eksen,tensor = None):
        if tensor == None:
            tensor = self.__veri
            
        result = initial_value

        if eksen>0:
            for element in tensor:
                result = self.avg_eksen(result,eksen-1, element)
        else:
            single_element = []
            for element in tensor:
                single_element.append(element)
                
            single_result = 0
            summation = lambda x, y: x + y
            for element in single_element:
                single_result = self._element_wise_operation(single_result, summation, False, element)
            result.append(single_result)
        return result
    
    def ortalama(self, eksen=None):
        if eksen is not None and not isinstance(eksen, int):
            raise TypeError("Eksen must be an integer or None.")
        
        #Calculates the average of the elements of the gergen object, optionally along a specified axis 'eksen'.
        if eksen == None:
            summation = lambda x, y: x + y
            summed_gergen = self._apply_operation_all_elmts(summation,0)
            element_count = self.uzunluk()
            result = summed_gergen/element_count
        else:
            if eksen>len(self.boyut()):
                raise ValueError("Specified eksen is out of bounds")
            average = self.avg_eksen([], eksen)
            new_dim = []
            dim = self.boyut()
            for i in range(0, len(dim)):
                if i == eksen:
                    continue
                else:
                    new_dim.append(dim[i])
            average = gergen(average).duzlestir()
            summation_matrix = self.format_matrix(average.veri(), new_dim)
            #_apply_to_elements(self, operation, veri = None, operand = None): 
            operation = lambda x, y: x / y
            result = self._apply_to_elements(operation, summation_matrix, dim[eksen])
            result = gergen(result)
            
            
        return result
    

    
def example_1():
    #Example 1
    boyut = (64,64)
    A = rastgele_gercek(boyut)
    B = rastgele_gercek(boyut)

    start = time.time()
    #TODO
    #Apply given equation
    AT = A.devrik()
    gergen_result = AT.ic_carpim(B)
    end = time.time()

    a = np.random.rand(64,64)
    b = np.random.rand(64,64)
    start_np = time.time()
    #Apply the same equation for NumPy equivalent
    aT = a.transpose()
    np_result = np.dot(aT,b)
    end_np = time.time()

    #TODO:
    #Compare if the two results are the same
    #Report the time difference
    print("Time taken for gergen:", end-start)
    print("Time taken for numpy:", end_np-start_np)

    """
    Time taken for gergen: 0.015883922576904297
    Time taken for numpy: 3.886222839355469e-05
    
    Result for numpy is significantly less than gergen class.
    Probably numpy is more efficient in calculations.
    """
    
def example_2():
    #Example 2
    #TODO:
    boyut = (4,16,16,16)
    A = rastgele_gercek(boyut)
    B = rastgele_gercek(boyut)
    C = rastgele_gercek(boyut)
    
    #gergen time
    start = time.time()
    
    AmulB = A.__mul__(B)
    CmulA = C.__mul__(A)
    BmulC = B.__mul__(C)
    
    ABsumCA = AmulB.__add__(CmulA)
    matrix_res = ABsumCA.__add__(BmulC)
    
    gergen_result = matrix_res.ortalama()
    
    end = time.time()
    
    a = np.random.rand(4,16,16,16)
    b = np.random.rand(4,16,16,16)
    c = np.random.rand(4,16,16,16)
    #numpy time
    start_np = time.time()
    #Apply the same equation for NumPy equivalent
    axb = np.multiply(a, b)
    cxa = np.multiply(c,a)
    bxc = np.multiply(b,c)
    
    axbcxa = np.add(axb, cxa)
    matrix_result = np.add(axbcxa, bxc)
    
    numpy_result = matrix_result.mean()
    
    end_np = time.time()
    
    print("Time taken for gergen:", end-start)
    print("Time taken for numpy:", end_np-start_np)

    
    """
    Time taken for gergen: 0.027724742889404297
    Time taken for numpy: 0.0003571510314941406
    
    Result for numpy is significantly less than gergen class.
    Probably numpy is more efficient in calculations.
    
    """
    
    return gergen_result

def example_3():
    #Example 3
    #TODO:
    boyut = (3,64,64)
    A = rastgele_gercek(boyut)
    B = rastgele_gercek(boyut)

    start = time.time()
    #TODO
    #Apply given equation
    sinA = A.sin()
    cosB = B.cos()
    sinAcosB = sinA.__add__(cosB)
    
    lnned = sinAcosB.ln()
    squared = lnned.us(2)
    
    gergen_result = squared.__truediv__(8)
    
    end = time.time()

    a = np.random.rand(3,64,64)
    b = np.random.rand(3,64,64)
    start_np = time.time()
    #Apply the same equation for NumPy equivalent
    sina = np.sin(a)
    cosb = np.cos(b)
    sinacosb = np.add(sina,cosb)
    
    lnnednp = np.log(sinacosb)
    
    squarednp = np.square(lnnednp)
    
    np_result = np.divide(squarednp, 8)
    
    
    end_np = time.time()
    #TODO:
    #Compare if the two results are the same
    #Report the time difference
    print("Time taken for gergen:", end-start)
    print("Time taken for numpy:", end_np-start_np)
    
    """
    Time taken for gergen: 0.016682863235473633
    Time taken for numpy: 0.00025081634521484375
    
    Again, numpy is very successful in computing. 
    This is caused of efficiency issues in my implementation.
    """
    return gergen_result
    
if __name__ == '__main__':
    """
    example_1()
    example_2()
    example_3()
    """
    tester()
    