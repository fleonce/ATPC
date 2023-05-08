#include <iostream>
#include <thread>
#include "openmpi/mpi.h"

template<class T>
class DistObj {
private:
    T _obj;
    MPI_Comm comm{MPI_COMM_WORLD};
    MPI_Datatype datatype{MPI_DATATYPE_NULL};
    MPI_Win window{MPI_WIN_NULL};
    int rank{-1};
    int local_tokens{1};
    int total_tokens{0};
    int *status;
    int *tokens;
    int free_tokens;
    int used_tokens{0};

    int world_size;

public:
    DistObj(T &val, MPI_Comm comm, MPI_Datatype datatype) {
        this->comm = comm;
        this->_obj = val;
        this->local_tokens = 1;
        this->datatype = datatype;
        MPI_Comm_rank(comm, &this->rank);
        MPI_Comm_size(comm, &this->total_tokens);
        MPI_Comm_size(comm, &this->world_size);
        this->free_tokens = this->world_size;  // there are |world_size| free tokens available
        this->status = new int[2]{0};
        /*MPI_Win_create(
                (void*)this->status,
                (this->total_tokens + 1) * sizeof(int),
                sizeof(int), MPI_INFO_NULL,
                MPI_COMM_WORLD,
                &(this->window));*/
        // todo shared mem with size world_size and each entry has token count for each rank
    }

    void alloc(int n) {
        int data[3]{this->free_tokens, 0, 0};
        int flag;
        do {
            MPI_Request request;
            MPI_Status state;
            MPI_Irecv(&data, 3, MPI_INT, MPI_ANY_SOURCE, 0, this->comm, &request);
            MPI_Test(&request, &flag, &state);
            if(flag) {
                // std::cout << "RANK " << this->rank << ": " << "Received " << data[0] << " from " << state.MPI_SOURCE << "\n";
                if (data[1] == 1 and data[2] >= this->world_size) {
                    // if action was a release and more than 1 token has been released, it was a write
                    MPI_Recv(&this->_obj, 1, this->datatype, state.MPI_SOURCE, 1, this->comm, MPI_STATUS_IGNORE);
                    // std::cout << "RANK " << this->rank << ": " << "Rank " << state.MPI_SOURCE << " issued a write, receiving.. Done.\n";
                    continue;
                }
            }
        } while (flag or data[0] < n or (flag and data[1] == 0));

        data[0] -= n;
        data[1] = 0;
        this->free_tokens = data[0];
        this->used_tokens = n;
        int update[]{data[0] - n, 0, 0};
        // std::cout << "RANK " << this->rank << ": " << "Allocated " << n << " Free " << this->free_tokens << "\n";
        for (int i = 0; i < this->world_size; i++) {
            MPI_Request request;
            MPI_Isend(&update, 3, MPI_INT, i, 0, this->comm, &request);
        }
    }

    void free(int n) {
        int data[3]{this->free_tokens + n, 1, n};
        for(int i = 0; i < this->world_size; i++) {
            MPI_Request request;
            MPI_Isend(&data, 3, MPI_INT, i, 0, this->comm, &request);
        }
        this->used_tokens = 0;
    }

    T read() {
        alloc(1);
        T val = this->_obj;
        free(1);
        return val;
    }

    void write(T &val) {
        alloc(this->world_size);
        for (int i = 0; i < this->world_size; i++) {
            MPI_Request request;
            MPI_Isend(&val, 1, this->datatype, i, 1, this->comm, &request);
        }
        this->_obj = val;
        free(this->world_size);
    }

    T read2() {
        std::cout << this->rank << ": " << "read() started\n";
        int rk;
        MPI_Comm_rank(this->comm, &rk);
        MPI_Win_fence(0, this->window);
        int swap = 1;
        int result;
        int compare = 0;
        do {
            MPI_Compare_and_swap(
                    &swap,
                    &compare,
                    &result,
                    MPI_INT,
                    0,
                    0,
                    this->window
            );
            std::cout << this->rank << ": " << "Waiting for free channel: " << compare << " " << result << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } while(compare != result);
        int magic = this->rank == 0 ? 42 : -1;
        MPI_Put(&magic, 1, MPI_INT, 0, 1, 1, MPI_INT, this->window);
        int res = -111;
        MPI_Get(&res, 1, MPI_INT, 0, 1, 1, MPI_INT, this->window);
        std::cout << this->rank << ": " << "Channel is free: " << compare << " " << result << " Res:" << res << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        int val = 0;
        MPI_Put(&val, 1, MPI_INT, 0, 0, 1, MPI_INT, this->window);
        return res;
    }
};


void check_err(int err, const std::string& msg) {
    if (err != 0) {
        std::cout << "ERR: " << err << " " << msg << "\n";
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    check_err(MPI_Init(&argc, &argv), "MPI_Init");
    int rank, world_size;
    check_err(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "MPI_Comm_size");
    check_err(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "MPI_Comm_rank");

    int init = 0;
    DistObj<int> dist = DistObj<int>(init, MPI_COMM_WORLD, MPI_INT);
    if(rank == 0) {
        std::cout << "World size: " << world_size << "\n";
    }
    int update = rank == 1 ? 42 : -1;
    dist.write(update);
    int val = dist.read();
    std::cout << "RANK " << rank << " | Read value " << val << "\n";

    check_err(MPI_Finalize(), "MPI_Finalize");
    return 0;
}
