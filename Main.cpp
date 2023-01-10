#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <random>
#include <omp.h>
#include <cstdlib>
#include <time.h>
#include "sciplot/sciplot.hpp"

using namespace sciplot;

using namespace std;


/*
    Matrix multiplication A x B = result
    Using OpenMP for parallel execution operations

    Author: Ilya Kosarev
    IVT-41BO
*/

// Init app by default settings for language and rand
void init_app();

// Create matrix with size rows x cols for value with min_value to max_value
int** create_matrix(int rows, int cols, int min_value, int max_value);

// Delete matrix with size rows x cols from memory
void delete_matrix(int** matrix, int rows, int cols);

// Fill matrix with size rows x cols by values from min_value to max_value
int** fill_matrix(int** matrix, int rows, int cols, int min_value, int max_value);

// Create and fill matrix with size rows x cols by random values from 0 to 13
int** create_fill_matrix(int rows, int cols, int min_value, int max_value);

// Display matrix with size rows x cols on console
void display_matrix(int** matrix, int rows, int cols);

// Generate graph x, y
void draw_graph(vector<double> x, vector<double> y);

int main(int argc, char** argv) {
    init_app();

    // MATRICES SIZE
    // A matrix size
    const int rowsA = 740;
    const int colsA = 770;

    // B matrix size
    const int rowsB = colsA;
    const int colsB = 780;

    // result matrix size
    const int result_rows = rowsA;
    const int result_cols = colsB;

    // consts for app
    const int MIN_MATRIX_VALUE = 0;
    const int MAX_MATRIX_VALUE = 8;
    const int MAX_THTREAD_NUMBER = 4;

    // Lists of past time by current count
    vector<double> past_times_values = {};
    vector<double> thread_count_values = {};

    // reset auto thread num in openmp
    omp_set_dynamic(0);

    // cycle threads from 1 to MAX_THREAD_COUNT
    for (int thread_count = 1; thread_count <= MAX_THTREAD_NUMBER; thread_count++) {
        // set thread num
        omp_set_num_threads(thread_count);

        // Filling matrices
        // A matrix
        int** matrix_A = create_fill_matrix(rowsA, colsA, MIN_MATRIX_VALUE, MAX_MATRIX_VALUE);
        // B matrix
        int** matrix_B = create_fill_matrix(rowsB, colsB, MIN_MATRIX_VALUE, MAX_MATRIX_VALUE);
        // result matrix with init 0 values
        int** matrix_result = create_fill_matrix(result_rows, result_cols, 0, 0);

        // Multiplication matrices
        int i, j, k;
        double start_datetime = omp_get_wtime();
        
        #pragma omp parallel for shared(matrix_A, matrix_B, matrix_result) private(i, j, k)
        for (i = 0; i < rowsA; i++) {
            for (j = 0; j < colsB; j++) {
                for (k = 0; k < colsA; k++) {
                    matrix_result[i][j] += matrix_A[i][k] * matrix_B[k][j];
                }
            }
        }
        double end_datetime = omp_get_wtime();
        double past_time = end_datetime - start_datetime;

        cout << "Current used thread count: " << thread_count << endl;
        cout << "Past time:  " << past_time << " seconds\n" << endl;

        // Adding values for graphics
        past_times_values.push_back(past_time);
        thread_count_values.push_back(thread_count);

        // Clear all matrices
        delete_matrix(matrix_A, rowsA, colsA);
        delete_matrix(matrix_B, rowsB, colsB);
        delete_matrix(matrix_result, result_rows, result_cols);
    }

    draw_graph(thread_count_values, past_times_values);

    return 0;
}


// Init app by default settings for language and rand
void init_app() {
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));
}

// Display matrix with size rows x cols on console
void display_matrix(int** matrix, int rows, int cols) {
    cout << "\nMatrix (" << rows << " x " << cols << "):" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << "  " << matrix[i][j];
        }
        cout << "\n";
    }
    cout << "\n";
}

// Create matrix with size rows x cols
int** create_matrix(int rows, int cols) {
    int** matrix;
    matrix = new int* [rows];

    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
    }
    return matrix;
}

// Fill matrix with size rows x cols by values from min_value to max_value
int** fill_matrix(int** matrix, int rows, int cols, int min_value, int max_value) {
    // init_randomizer
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min_value, max_value);

    // fill matrix by random_values
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = distr(gen);
        }
    }
    return matrix;
}


void delete_matrix(int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++)
        delete[] matrix[i];
    delete[] matrix;
}

// Create and fill matrix with size rows x cols by random values from min_value to max_value 
int** create_fill_matrix(int rows, int cols, int min_value, int max_value) {
    int** matrix = create_matrix(rows, cols);
    matrix = fill_matrix(matrix, rows, cols, min_value, max_value);
    return matrix;
}

// Generate graph x, y
void draw_graph(vector<double> x, vector<double> y) {
    Plot2D plot;

    // Set the x and y labels
    plot.xlabel("Threads");
    plot.ylabel("Time");

    plot.legend()
        .hide();

    // Plot the functions
    plot.drawCurve(x, y);

    // Create figure to hold plot
    Figure fig = { {plot} };
    // Create canvas to hold figure
    Canvas canvas = { {fig} };

    canvas.size(800, 800);

    // Show the plot in a pop-up window
    canvas.show();

    // Save the plot to a PNG file
    canvas.save("graph.png");
}