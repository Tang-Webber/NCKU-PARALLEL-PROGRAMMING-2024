#include <omp.h>
#include <iostream>
using namespace std;
#include <limits.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include<algorithm>

// overloading cout << for vector
// for printing out all permutation case at line 40-42
template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    os << "[";
    for (int i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i != v.size() - 1)
            os << ", ";
    }
    os << "]";
    return os;
}

vector<vector<int>> produce_a_vector_containing_all_permutation(int max_task_number){

    vector<vector<int>> all_permutation;
    all_permutation.clear();
    vector<int> permutation_case;
    permutation_case.clear();

    for(int i = 0;i < max_task_number;i++){
        permutation_case.push_back(i);
    }

    int count = 0;
    do{
        all_permutation.push_back(permutation_case);
        // std::cout << count << " th permutation:";
        // cout << all_permutation.back()<< endl;
        // count++;
    }
    while(next_permutation(permutation_case.begin(),permutation_case.end()));

    return all_permutation;
}

int main()
{
    // -------------------------------
    // basic read file
    // -------------------------------
    string input_file_name = "";
    cin >> input_file_name;
    
    std::ifstream myfile; 
    myfile.open(input_file_name);

    int max_task_number = 0;
    myfile >> max_task_number;

    int pi;//cost
    int ri;//release time
    int wi;//weight
    int max_ri = 0;
    int sum_of_all_pi = 0;
    vector<vector<int>> task_list;
    vector<int> permutation_case;

    int cnt = 0;
    while ( myfile ) {
        
        myfile >> pi >> ri >> wi;
        vector<int> tmp {pi , ri , wi};
        task_list.push_back(tmp);
        cnt++;
        if(ri > max_ri) max_ri = ri;
        sum_of_all_pi += pi;

        if(cnt == max_task_number) break;
    }
    myfile.close();

    // -------------------------------
    // produce_a_vector_containing_all_permutation
    // -------------------------------
    vector<vector<int>> all_permutation_list = produce_a_vector_containing_all_permutation(max_task_number);

    // -------------------------------
    // processing all the case
    // if got n tasks,then should have n! permutation case
    // -------------------------------
    int global_min_weighted_cost = INT_MAX;
    int schedule_gantt_chart_length = max_ri+sum_of_all_pi;
#pragma omp parallel for
    for(int i = 0;i < all_permutation_list.size();i++){
        // all_permutation_list[i].size() == max_task_number
        int schedule_gantt_chart[schedule_gantt_chart_length];

        for(int j = 0;j < schedule_gantt_chart_length;j++){
            // -1 means this moment is not occupied by any tesk
            schedule_gantt_chart[j] = -1;
        }

        int cur_total_weighted_cost = 0;
        for(int j = 0;j < max_task_number;j++){
            // elenment in task_list is vector containing {pi , ri , wi}
            int cur_task_id = all_permutation_list[i][j];
            int cur_pi = task_list[cur_task_id][0];
            int cur_ri = task_list[cur_task_id][1];

            while(cur_pi){
                if(schedule_gantt_chart[cur_ri] == -1){
                    schedule_gantt_chart[cur_ri] = cur_task_id;
                    cur_pi--;
                }
                cur_ri++;
            }
            // after fill current taks into schedule_gantt_chart,
            // calculate and sum to current total weighted cost
            if(cur_pi == 0)
                cur_total_weighted_cost += cur_ri*task_list[cur_task_id][2];
            // cout << cur_ri <<" " << task_list[cur_task_id][2] <<" - ";    
        }
        // refresh the min
        #pragma omp critical
        {
            if(global_min_weighted_cost > cur_total_weighted_cost)
                global_min_weighted_cost = cur_total_weighted_cost;            
        }
    }
    cout << global_min_weighted_cost;
    return 0; 
}