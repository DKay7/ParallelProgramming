#include <fstream>
#include <mpi.h>
#include <vector>
#include <string>

const int MIN_DATA_SIZE = 100;
const int MAX_DATA_SIZE = 1000000;
const int NUM_REPEATS = 100;

void server(std::ofstream &out_file, std::vector<std::string> &messages) {
    double start_time = 0;
    double end_time = 0;

    for (auto &message : messages) {
        double deltaTime = 0;

        for (int repeat = 0; repeat < NUM_REPEATS; ++repeat) {
            start_time = MPI_Wtime();

            MPI_Send((void *)message.c_str(), message.capacity(), MPI_CHAR, 1,
                     0, MPI_COMM_WORLD);
            MPI_Recv((void *)message.c_str(), message.capacity(), MPI_CHAR, 1,
                     0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            end_time = MPI_Wtime();
            deltaTime += end_time - start_time;
        }

        out_file << message.capacity() << " " << deltaTime << "\n";
    }
}

void client(std::vector<std::string> &messages) {
    for (auto& message : messages) {
        for (int repeat = 0; repeat < NUM_REPEATS; ++repeat) {
            MPI_Recv((void*) message.c_str(), message.capacity(), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send((void*) message.c_str(), message.capacity(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        
        }
  }
}

std::vector<std::string> create_messages() {
    std::vector<std::string> messages{};

    for (int msg_size = MIN_DATA_SIZE; msg_size < MAX_DATA_SIZE; msg_size += MIN_DATA_SIZE) {
        messages.emplace_back(msg_size, 'a');
    }

    return messages;
}

int main(int argc, char **argv) {
    int rank = 0, size = 0;
    std::ofstream out_file("time-experiment.txt");

    auto messages = create_messages();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
        server(out_file, messages);

    else if (rank == 1)
        client(messages);

    MPI_Finalize();
    return 0;
}
