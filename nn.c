#include "nn.h"

#include <stdint.h>

// Neural Network architecture
#define NN_LAYER_0_NEURONS (2)
#define NN_LAYER_1_NEURONS (1)


// Quantized NN Model Parameters
const int8_t l0_qweights[2][2] = {
	{ -49, 31, },
	{ -48, 30, },
};

const int8_t l0_qbiases[2] = { 26, -31, };

const int8_t l1_qweights[2][1] = {
	{ -60, },
	{ -49, },
};

const int8_t l1_qbiases[1] = { 1, };

// Quantized lookup table for qelu activation function
// Look up quantized result of input argument by casting the
// int8_t (qm.n) to uint8_t for the lookup table index, e.g.:
// int8_t qresult = qelu_lut[(uint8_t)qargument];

static const int8_t qelu_lut[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 
	8, 9, 10, 11, 12, 13, 14, 15, 
	16, 17, 18, 19, 20, 21, 22, 23, 
	24, 25, 26, 27, 28, 29, 30, 31, 
	32, 33, 34, 35, 36, 37, 38, 39, 
	40, 41, 42, 43, 44, 45, 46, 47, 
	48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 58, 59, 60, 61, 62, 63, 
	64, 65, 66, 67, 68, 69, 70, 71, 
	72, 73, 74, 75, 76, 77, 78, 79, 
	80, 81, 82, 83, 84, 85, 86, 87, 
	88, 89, 90, 91, 92, 93, 94, 95, 
	96, 97, 98, 99, 100, 101, 102, 103, 
	104, 105, 106, 107, 108, 109, 110, 111, 
	112, 113, 114, 115, 116, 117, 118, 119, 
	120, 121, 122, 123, 124, 125, 126, 127, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -16, -16, -16, -16, -16, -16, -16, 
	-16, -15, -15, -15, -15, -15, -15, -15, 
	-15, -15, -15, -15, -15, -15, -15, -15, 
	-15, -15, -15, -14, -14, -14, -14, -14, 
	-14, -14, -14, -13, -13, -13, -13, -13, 
	-12, -12, -12, -12, -11, -11, -11, -10, 
	-10, -10, -9, -9, -8, -8, -7, -7, 
	-6, -6, -5, -4, -4, -3, -2, -1, 
	};

// Quantized lookup table for qsigmoid activation function
// Look up quantized result of input argument by casting the
// int8_t (qm.n) to uint8_t for the lookup table index, e.g.:
// int8_t qresult = qsigmoid_lut[(uint8_t)qargument];

static const int8_t qsigmoid_lut[256] = {
	8, 8, 8, 9, 9, 9, 9, 10, 
	10, 10, 10, 11, 11, 11, 11, 11, 
	12, 12, 12, 12, 12, 13, 13, 13, 
	13, 13, 13, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 3, 3, 
	3, 3, 3, 3, 4, 4, 4, 4, 
	4, 5, 5, 5, 5, 5, 6, 6, 
	6, 6, 7, 7, 7, 7, 8, 8, 
	};


//
// Quantized Model
//

// TODO: if you use your own implementation, replace my analysis with your own!
//
// Quantize the floating-point based NN architecture to work with int8_t,
// which is an 8-bit signed value ranging from -128 to 127.  Characterize the 
// maximum/minium value through your NN.  
//
// Hidden Layer (input range 0 to 1.1):
// 
// Both scale factors for the first neuron are ~ -3 so my accumulated sum of scaled
// inputs, worst case, is about -6.  But the bias is ~ +1.5 so
// this neuron's value into the activation function will range from about -4.5 to about 1.5
// its activation function is elu, which clamps negative values, 
// so after elu() this neuron's value ranges ~-1 to 1.5
//
// Both scale factors for the second neuron are ~2 so my accumulated sum of scaled
// inputs, worst case, is about 4, but the bias is ~ -2 so
// this neuron's value into the activation function will range from about -2 to about 2
// its activation function is elu, so again the neuron's final value ranges from ~-1 to ~2
//
// So, back of napkin, the worst-case intermediate value is ~-6 and the final neuron 
// values, which become inputs to the next layer, are within ~-1 to ~2
// 
// Output Layer (input range approx -1 to 2)
// 
// The scale factors are both negative, around -4 and -3 respectively.
// My worst-case accumulated sum of scaled inputs is -6, and the bias is 
// approximately zero, so this neuron's largest value into the activation
// function is ~-6.  
// its activation function is sigmoid, which ranges from 0 to 1
//
// So the worst, case value in my system has a magnitude of 6, which I can encode in 3 bits.
//
// This suggests I can get away with Q3.4 fixed-point representation, but I don't want any 
// trouble while I'm debugging so I'm going to hedge my bet and start out with Q4.3
#define QNN_FRACTIONAL_BITS (4)

// TODO: Implement this function using fixed-point numbers/arithmetic - no floating point!
// TODO: Comment the function as if I were sitting next to you and you are walking me through
// your implementation - what are your intentions, how do you implement them, what design 
// decisions did you think about, and then make, and why?
// Run the quantized neural network using only fixed-point arithmetic.
// This function is similar to NN_predict(), but uses fixed point arithmetic

static void NN_qcompute_layer(const int8_t *inputs_q, int8_t *outputs_q, const int8_t *weights_q, const int8_t *biases_q, int inputs, int neurons, const int8_t *activation_lut)
{
    // going over each neuron in the layer
    for (int j = 0; j < neurons; j++) {

        // Start with the bias term.
        // Bias is already in Q3.4.
        int16_t qacc = (int16_t)biases_q[j];

        // collect and add weighted inputs
        for (int k = 0; k < inputs; k++) {

            // Multiply input (Q3.4) by weight (Q3.4)
            // Result is Q6.8 and must be held in int16_t
            int16_t qmult =
                (int16_t)weights_q[k * neurons + j] * (int16_t)inputs_q[k];

            // Shift right by fractional bits to bring Q6.8 → Q3.4
            qmult >>= QNN_FRACTIONAL_BITS;
            qacc += qmult;
        }

        // Apply activation function via lookup table.
        // Cast to uint8_t to correctly map signed Q3.4 range
        // into LUT indices [0–255].
        outputs_q[j] = activation_lut[(uint8_t)qacc];
    }
}
int8_t NN_qpredict(const int8_t *input_qvalues)
{
    // Hidden layer neuron outputs (Q3.4)
    int8_t l0_qneurons[NN_LAYER_0_NEURONS];

    // Compute hidden layer using ELU lookup table
    NN_qcompute_layer(input_qvalues, l0_qneurons, (const int8_t *)l0_qweights, l0_qbiases, NN_INPUTS, NN_LAYER_0_NEURONS, qelu_lut);

    // Output layer has a single neuron (Q3.4)
    int8_t l1_qneuron;

    // Compute output layer using sigmoid lookup table
    NN_qcompute_layer(l0_qneurons, &l1_qneuron, (const int8_t *)l1_qweights, l1_qbiases, NN_LAYER_0_NEURONS, NN_LAYER_1_NEURONS, qsigmoid_lut);

    // Return quantized output
    return l1_qneuron;
}
