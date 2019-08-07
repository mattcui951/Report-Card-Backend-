#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <algorithm>
using namespace std;
int main() {
    ifstream ip1("courses.csv");
    if(!ip1.is_open()) cerr<<"courses.csv not found"<<endl;
    ifstream ip2("marks.csv");
    if(!ip2.is_open()) cerr<<"marks.csv not found"<<endl;
    ifstream ip3("students.csv");
    if(!ip3.is_open()) cerr<<"students.csv not found"<<endl;
    ifstream ip4("tests.csv");
    if(!ip4.is_open()) cerr<<"tests.csv not found"<<endl;
////////////////////////////////////////////////////
    map<int, vector<string>> courses;
    string tmp1;
    string tmp2;
    string tmp3;
    getline(ip1,tmp1);

    while(!ip1.eof()){
        vector<string> courses_vec;
        getline(ip1, tmp1, ',');
        getline(ip1, tmp2, ',');
        getline(ip1, tmp3, '\n');
        if(tmp1.length()==0) break;
        courses_vec.push_back(tmp2);
        courses_vec.push_back(tmp3);
        courses[stoi(tmp1)] = courses_vec;
        courses_vec.clear();
    }
    ip1.close();

    map<pair<int,int>, string> marks;
    getline(ip2,tmp1);
    while(!ip2.eof()){
        getline(ip2, tmp1, ',');
        if(tmp1.length()==0) break;
        getline(ip2, tmp2, ',');
        getline(ip2, tmp3, '\n');
        marks[make_pair(stoi(tmp1),stoi(tmp2))] = tmp3;
    }
    ip2.close();

    map<int, string> students;
    getline(ip3,tmp1);
    while(!ip3.eof()){
        getline(ip3, tmp1, ',');
        if(tmp1.length()==0) break;
        getline(ip3, tmp2, '\n');
        students[stoi(tmp1)]=tmp2;
    }
    ip3.close();

    getline(ip4,tmp1);
    map<int, vector<string>> tests;
    while(!ip4.eof()){
        vector<string> test_vec;
        getline(ip4, tmp1, ',');
        if(tmp1.length()==0) break;
        getline(ip4, tmp2, ',');
        getline(ip4, tmp3, '\n');
        test_vec.push_back(tmp2);
        test_vec.push_back(tmp3);
        tests[stoi(tmp1)] = test_vec;
    }
    ip4.close();
    // find courses that do not add up to 100 weight
    vector<int> error_course;
    for (map<int, vector<string>>::iterator i=courses.begin();i!=courses.end();i++ ){
        int cur_courseid=i->first;
        float sum = 0;
        for (map<int, vector<string>>::iterator j=tests.begin();  j!=tests.end() ; j++) {
            if(cur_courseid==stoi(j->second[0])) sum+=stof(j->second[1]);
        }
        if(abs(sum-100)>1) error_course.push_back(cur_courseid);
    }

    ofstream output_file("output.txt");

    map<int, map<int,float>> student_course_avg;
    map<int, float> std_total_avg;
    for (map<int,string>::iterator i = students.begin(); i != students.end() ; i++) {
        int cur_student_id = i->first;

        // merge course_id, test_id, test_mark and test_weight into one map.
        map<pair<int,int>, vector<string>> course_id_test_id_test_mark_weight;
        for(map<pair<int,int>,string>::iterator j=marks.begin();j!=marks.end();j++){
            pair<int,int> mypair= j->first;
            if(mypair.second==cur_student_id){
                vector<string> myvec;
                myvec.push_back(j->second);
                myvec.push_back(tests[mypair.first][1]);
                course_id_test_id_test_mark_weight[make_pair(stoi(tests[mypair.first][0]), mypair.first)]=myvec;
            }
        }

        vector<int> error_course_id;
        for(map<pair<int,int>,vector<string>>::iterator j=course_id_test_id_test_mark_weight.begin();j!=course_id_test_id_test_mark_weight.end();j++){
            int cur_courseidd = j->first.first;
            for(map<int, vector<string>>::iterator k=tests.begin();k!=tests.end();k++){
                if(cur_courseidd == stoi(k->second[0])){
                    pair<int, int> tmp = make_pair(stoi(k->second[0]), k->first);
                    if(course_id_test_id_test_mark_weight.count(tmp) <=0){
                        output_file<<"Student " << to_string(cur_student_id) << " has not completed all the tests for Course: " << courses[cur_courseidd][0]<< endl;
                        error_course_id.push_back(cur_courseidd);
                    }
                }
            }
        }


        //calculate course avg for each student and store it in the map
        for(map<pair<int,int>,vector<string>>::iterator j=course_id_test_id_test_mark_weight.begin();j!=course_id_test_id_test_mark_weight.end();j++){
            int cur_course = j->first.first;
            int cur_test_id = j->first.second;
            float cur_test_mark = stof(j->second[0]);
            float cur_weight = stof(j->second[1]);
            if (student_course_avg[cur_student_id].count(cur_course)>0) student_course_avg[cur_student_id][cur_course] += roundf(cur_test_mark*cur_weight)/100;
            else student_course_avg[cur_student_id][cur_course] = roundf(cur_test_mark*cur_weight)/100;
        }

        //calculate total avg for each student and store it in the map
        for (map<int,float>::iterator j = student_course_avg[cur_student_id].begin(); j != student_course_avg[cur_student_id].end() ; j++) {
            if(std_total_avg.count(cur_student_id)>0) std_total_avg[cur_student_id] += roundf((j->second/student_course_avg[cur_student_id].size()) * 100)/100;
            else std_total_avg[cur_student_id] = roundf((j->second/student_course_avg[cur_student_id].size()) * 100)/100;
        }

        //write to output.txt file starting with total avg
        string outstring = "";
        if(error_course_id.size()>0){
            outstring+="Student Id: " + to_string(cur_student_id) + ", name: " + i->second + "\n" + "Total Average:    not calculated due to uncompleted courses\n\n";
        } else{
            outstring+= "Student Id: " + to_string(cur_student_id) + ", name: " + i->second + "\n" + "Total Average:    " + to_string(std_total_avg[cur_student_id])+"%\n\n" ;
        }

        if(output_file.is_open()){
            output_file<< outstring;
        }else{
            cerr<<"Unable to open output.txt file"<<endl;
        }
        // writing course avg
        for (map<int,float>::iterator j = student_course_avg[cur_student_id].begin(); j != student_course_avg[cur_student_id].end() ; j++) {
            if(output_file.is_open()){
                output_file<<"     Course: ";
                vector<int>::iterator it = find(error_course.begin(),error_course.end(),j->first);
                vector<int>::iterator it2 = find(error_course_id.begin(),error_course_id.end(),j->first);
                if(it == error_course.end() && error_course.size()!=0){
                    output_file << courses[j->first][0] << ", Teacher: " << courses[j->first][1]<<endl<<"     ERROR: Weights do not add up to 100"<< endl<<endl;
                } else if(it2 != error_course_id.end()){
                    output_file << courses[j->first][0] << ", Teacher: " << courses[j->first][1]<<endl<<"     Final Grade: do not completed"<<endl<<endl;
                }else{
                    output_file << courses[j->first][0] << ", Teacher: " << courses[j->first][1]<<endl<<"     Final Grade: "<< to_string(j->second)<< "%"<<endl<<endl;
                }
            } else{
                cerr<<"Unable to open output.txt file"<<endl;
            }

        }
    }
    output_file.close();
    return 0;
}