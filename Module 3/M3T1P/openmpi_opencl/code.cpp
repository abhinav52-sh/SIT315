#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <mpi.h>
#include <CL/opencl.h>

using namespace std;
using namespace std::chrono;

const int MATRIX_SIZE = 400;

int **Matrix1, **Matrix2, **Matrix3;

// Function to initialize a matrix with random values if initialize is true
void init_matrix(int **&matrix, int rows, int cols, bool initialize)
{
    matrix = new int *[rows];
    matrix[0] = new int[rows * cols];
    for (int i = 1; i < rows; ++i)
    {
        matrix[i] = matrix[i - 1] + cols;
    }

    if (initialize)
    {
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                matrix[i][j] = rand() % 100;
            }
        }
    }
}

// Function to perform matrix multiplication for a given range of rows
void multiply(int **matrix1, int **matrix2, int **matrix3, int num_rows)
{
    for (int i = 0; i < num_rows; ++i)
    {
        for (int j = 0; j < MATRIX_SIZE; ++j)
        {
            matrix3[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; ++k)
            {
                matrix3[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

// Function to print a matrix
void print_matrix(int **matrix, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << "--------------" << endl;
}

// Function executed by the head process
void head(int total_processes)
{
    init_matrix(Matrix1, MATRIX_SIZE, MATRIX_SIZE, true);
    init_matrix(Matrix2, MATRIX_SIZE, MATRIX_SIZE, true);
    init_matrix(Matrix3, MATRIX_SIZE, MATRIX_SIZE, false);

    int num_rows = MATRIX_SIZE / total_processes;
    int broadcast_size = (MATRIX_SIZE * MATRIX_SIZE);
    int scatter_gather_size = (MATRIX_SIZE * MATRIX_SIZE) / total_processes;

    auto start = high_resolution_clock::now();

    // Scatter Matrix1 to worker processes
    MPI_Scatter(&Matrix1[0][0], scatter_gather_size, MPI_INT, &Matrix1[0][0], 
                scatter_gather_size, MPI_INT, 0, MPI_COMM_WORLD);
                
    // Broadcast Matrix2 to all processes
    MPI_Bcast(&Matrix2[0][0], broadcast_size, MPI_INT, 0, MPI_COMM_WORLD);

    // OpenCL initialization
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    // Create an OpenCL context
    err = clGetPlatformIDs(1, &platform, nullptr);
    err |= clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, nullptr);
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to create OpenCL context" << endl;
        exit(EXIT_FAILURE);
    }

    // Create a command queue
    queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to create command queue" << endl;
        exit(EXIT_FAILURE);
    }

    // Load kernel source code
    string kernelSource =
        string("\n") +
        string("__kernel void multiply(__global int* matrix1, __global int* matrix2, __global int* matrix3) {\n") +
        string("   int row = get_global_id(0);\n") +
        string("   int col = get_global_id(1);\n") +
        string("   int sum = 0;\n") +
        string("   for (int i = 0; i < ") + to_string(MATRIX_SIZE) + string("; ++i)\n") +
        string("       sum += matrix1[row * ") + to_string(MATRIX_SIZE) + string(" + i] * matrix2[i * ") + to_string(MATRIX_SIZE) + string(" + col];\n") +
        string("   matrix3[row * ") + to_string(MATRIX_SIZE) + string(" + col] = sum;\n") +
        string("}\n");

    const char *KernelSource = kernelSource.c_str();

    // Create and build program
    program = clCreateProgramWithSource(context, 1, &KernelSource, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to create program" << endl;
        exit(EXIT_FAILURE);
    }

    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to build program" << endl;

        // Get build log
        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        char *log = new char[logSize];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log, nullptr);
        cerr << "Build log:\n"
             << log << endl;
        delete[] log;

        exit(EXIT_FAILURE);
    }

    // Create kernel
    kernel = clCreateKernel(program, "multiply", &err);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to create kernel" << endl;
        exit(EXIT_FAILURE);
    }

    // Create buffers
    cl_mem bufMatrix1 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * MATRIX_SIZE * MATRIX_SIZE, nullptr, &err);
    cl_mem bufMatrix2 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * MATRIX_SIZE * MATRIX_SIZE, nullptr, &err);
    cl_mem bufMatrix3 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * MATRIX_SIZE * MATRIX_SIZE, nullptr, &err);

    // Write data to buffers
    err = clEnqueueWriteBuffer(queue, bufMatrix1, CL_TRUE, 0, sizeof(int) * MATRIX_SIZE * MATRIX_SIZE, &Matrix1[0][0], 0, nullptr, nullptr);
    err |= clEnqueueWriteBuffer(queue, bufMatrix2, CL_TRUE, 0, sizeof(int) * MATRIX_SIZE * MATRIX_SIZE, &Matrix2[0][0], 0, nullptr, nullptr);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to write buffers" << endl;
        exit(EXIT_FAILURE);
    }

    // Set kernel arguments
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufMatrix2);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufMatrix3);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to set kernel arguments" << endl;
        exit(EXIT_FAILURE);
    }

    // Enqueue kernel
    size_t globalSize[2] = {MATRIX_SIZE, MATRIX_SIZE};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalSize, nullptr, 0, nullptr, nullptr);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to enqueue kernel" << endl;
        exit(EXIT_FAILURE);
    }

    // Read data back
    err = clEnqueueReadBuffer(queue, bufMatrix3, CL_TRUE, 0, sizeof(int) * MATRIX_SIZE * MATRIX_SIZE, &Matrix3[0][0], 0, nullptr, nullptr);
    if (err != CL_SUCCESS)
    {
        cerr << "Failed to read buffer" << endl;
        exit(EXIT_FAILURE);
    }

    // Clean up OpenCL objects
    clReleaseMemObject(bufMatrix1);
    clReleaseMemObject(bufMatrix2);
    clReleaseMemObject(bufMatrix3);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: " << duration.count() << " Microseconds" << endl;
}

// Function executed by worker processes
void node(int total_processes, int rank)
{
    int num_rows = MATRIX_SIZE / total_processes;
    int broadcast_size = (MATRIX_SIZE * MATRIX_SIZE);
    int scatter_gather_size = (MATRIX_SIZE * MATRIX_SIZE) / total_processes;

    init_matrix(Matrix1, MATRIX_SIZE, MATRIX_SIZE, true);
    init_matrix(Matrix2, MATRIX_SIZE, MATRIX_SIZE, false);
    init_matrix(Matrix3, MATRIX_SIZE, MATRIX_SIZE, false);

    // Scatter Matrix1 to worker processes
    MPI_Scatter(nullptr, scatter_gather_size, MPI_INT, &Matrix1[0][0], scatter_gather_size, MPI_INT, 0, MPI_COMM_WORLD);
    // Broadcast Matrix2 to all processes
    MPI_Bcast(&Matrix2[0][0], broadcast_size, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char **argv)
{
    srand(time(0));

    int total_processes;
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
        head(total_processes);
    else
        node(total_processes, rank);

    MPI_Finalize();

    return 0;
}