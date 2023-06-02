#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <string.h>    // strcat 함수가 선언된 헤더 파일
#include <stdlib.h>    // malloc, free 함수가 선언된 헤더 파일
#include <time.h>

char* baseUrl = "https://nginx-nginx-r8xoo2mleehqmty.sel3.cloudtype.app/rate/";

char* fileName_String() {
    char file_name[20] = "";
    struct tm* t;
    time_t base = time(NULL);
    t = localtime(&base);
    
    int year = t->tm_year + 1900;
    int month = t->tm_mon + 1;
    int day = t->tm_mday;
    
    char year_string[10];
    char month_string[10];
    char day_string[10];

    //sprintf(year_string, "%s", year);
    //sprintf(month_string, "%s", month);
    //sprintf(day_string, "%s", day);
    
    _itoa(year, year_string, 10);
    _itoa(month, month_string, 10);
    _itoa(day, day_string, 10);
   

    strcat(file_name, year_string);
    strcat(file_name, month_string);
    strcat(file_name, day_string);
    strcat(file_name, ".json");



    printf("%s\n", file_name);

    return file_name;

    
}

void save_jobjFile(char * jsonData) {
    char *file_name= NULL;
    file_name= fileName_String();
    printf("%s\n", file_name);

    //FILE* fp = fopen(fileName_String(), "w"); //test파일을 w(쓰기) 모드로 열기
    //fputs(jsonData,fp);
    //fclose(fp);
}

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t realsize = size * nmemb;
    char* data = (char*)userdata;
    /* 받은 데이터를 userdata에 추가 */
    memcpy(data, ptr, realsize);

    return realsize;
}

void exchange_api_request(char *currencyCode,char* selectCode,int money){
    printf("%s\n", currencyCode);
    CURL* curl;
    CURLcode res;

    //요청 서버 url
    char *url = malloc(sizeof(char) * 80);

    strcpy(url, baseUrl);

    strcat(url, currencyCode);

    


    printf("baseurl:%s\n", url);
    
    //요청 결과값 저장하기위한 버퍼 크기 지정
    char* response_buf = (char*)malloc(2048 * 2048);


    //헤더 변수
    struct curl_slist* headers = NULL;

    //응답 json 데이터
    //struct json_object* reponse_msg;
    
    //헤더 설정
    headers = curl_slist_append(headers, "Content-Type: application/json");

    //curl 전체 초기화
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl) {
        //api url 설정
        curl_easy_setopt(curl, CURLOPT_URL, url);

        //curl 헤더 설정 적용
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        //요청후 respons 값 처리 과정을 지정하는 코드
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        //respons 값 버퍼에 저장하기 위한 코드
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response_buf);

        //요청 시작하는 코드
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            
            printf("요청실패\n");

            //요청 실패시 실행
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            
        }
        else {
            //요청 성공
            
            printf("요청성공\n");

            //요청 성공시 오는 json값 글자 가져오기 위한 코드
            struct json_object* jobj = json_tokener_parse(response_buf);

            //환율 비율 json 값 가져오기 위한 구조체
            struct json_object* exchang_rate_obj;

            //전체 환율 비율을 가져오기 위한 구조체 {ex(USD,JPY,KRW,etc)}
            struct json_object* all_rate_obj;

            //특정 환율 비율을 가져오기 위한 구조체
            struct json_object* rate_obj;



            





            json_object_object_get_ex(jobj, "rate_data", &exchang_rate_obj);

            json_object_object_get_ex(exchang_rate_obj, "rates",&all_rate_obj);
            
            json_object_object_get_ex(all_rate_obj, selectCode, &rate_obj);

            char *save_data_string =json_object_get_string(exchang_rate_obj);
            
            save_jobjFile(save_data_string);

            printf("환율 비율: %f\n", json_object_get_double(rate_obj));

            //환율비율
            double rate_value = json_object_get_double(rate_obj);

            //환율 계산 결과
            double exchang_result = rate_value * money;

            printf("계산결과: %f\n", exchang_result);
            

        }
    }

    free(url);
    free(response_buf);


}


int main() {
    //KRW 베이스 돈,USD 환전하려는 나라의 돈,5000 얼마나 환전할건지
    exchange_api_request("KRW","USD",5000);
    system("pause > nul");
	return 0;
}