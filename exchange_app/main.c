#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <string.h>    // strcat 함수가 선언된 헤더 파일
#include <stdlib.h>    // malloc, free 함수가 선언된 헤더 파일
#include <time.h>


//유지보수
char* baseUrl = "https://nginx-nginx-r8xoo2mleehqmty.sel3.cloudtype.app/rate/";


struct CurrencyCode
{
    char JPY[4];
    char USD[4];
    char KRW[4];
};

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t realsize = size * nmemb;
    char* data = (char*)userdata;
    /* 받은 데이터를 userdata에 추가 */
    memcpy(data, ptr, realsize);

    return realsize;
}

struct CurrencyCode initStruct() {
    struct CurrencyCode code;

    strcpy(code.JPY,"JPY");
    strcpy(code.KRW, "KRW");
    strcpy(code.USD, "USD");

    return code;

}

char* selectCode() {
    //가지고 있는 돈의 번호를 선택
    //ex 1.한국 2.미국 3.일본 등등
    int selectCodeNum;
    static char selectCodeString[4]="";

    struct CurrencyCode currencyCode;

    //구조체를 초기화 하는 함수를 실행해서
    //currencyCode 구조체에 값을 넣는다.
    currencyCode = initStruct();

    
    printf("가지고 있는 돈의 국가를 선택해주세요.\n");
    printf("1.한국 2.미국 3.일본:");
    scanf("%d", &selectCodeNum);

    if (selectCodeNum == 1) {
        strcpy(selectCodeString, currencyCode.KRW);
        return selectCodeString;
    }
    else if (selectCodeNum == 2) {
        strcpy(selectCodeString, currencyCode.USD);
        return selectCodeString;
    }
    else if (selectCodeNum == 3) {
        strcpy(selectCodeString, currencyCode.JPY);
        return selectCodeString;
    }
}

char* targetSelectCode() {
    //가지고 있는 돈의 번호를 선택
    //ex 1.한국 2.미국 3.일본 등등
    int targetSelectCodeNum;
    static char targetSelectCodeString[4] = "";

    struct CurrencyCode currencyCode;

    //구조체를 초기화 하는 함수를 실행해서
    //currencyCode 구조체에 값을 넣는다.
    currencyCode = initStruct();


    printf("바꿀 돈의 국가를 선택해주세요.\n");
    printf("1.한국 2.미국 3.일본:");
    scanf("%d", &targetSelectCodeNum);

    if (targetSelectCodeNum == 1) {
        strcpy(targetSelectCodeString, currencyCode.KRW);
        return targetSelectCodeString;
    }
    else if (targetSelectCodeNum == 2) {
        strcpy(targetSelectCodeString, currencyCode.USD);
        return targetSelectCodeString;
    }
    else if (targetSelectCodeNum == 3) {
        strcpy(targetSelectCodeString, currencyCode.JPY);
        return targetSelectCodeString;
    }
}


//파일이름을 현재 년월일로 저장하기위해
//파일 이름을 만들어 주는 함수
char* fileName_String(char* currencyCode) {
    //함수가 끝나도 변수가 남아서 return 해줘야하기 때문에
    //static 정적 변수로 만들어 준다.
    char *file_name_pointer= malloc(sizeof(char) * 20);
    char file_name[20] = "";

    struct tm* t;
    time_t base = time(NULL);
    t = localtime(&base);

    int year = t->tm_year + 1900;
    int month = t->tm_mon + 1;
    int day = t->tm_mday;

    //년도 글자를 넣기위한 변수
    char year_string[10];
    //월 글자를 넣기 위한 변수
    char month_string[10];
    //일 글자를 넣기 위한 변수
    char day_string[10];

    //년도 숫자형을 문자형으로 변경하기위한 함수
    //숫자형,문자형을 넣기위한 변수,진수(8진수,16진수,10진수)
    _itoa(year, year_string, 10);
    _itoa(month, month_string, 10);
    _itoa(day, day_string, 10);


    //문자 연결하기 위한 함수
    strcat(file_name, year_string);
    strcat(file_name, month_string);
    strcat(file_name, day_string);
    strcat(file_name, currencyCode);
    strcat(file_name, ".json");

    //동적메모리는 포인터를 활용하므로
    //동적메모리로 지정된 변수에 글자를 집어넣으려면
    //넣을 값을 복사해서 동적메모리 변수에 넣어야한다.
    strcpy(file_name_pointer, file_name);

    return file_name_pointer;
}

//환율 비율 저장하기 위한 함수
void save_jobjFile(char* jsonData,char* currencyCode) {
    char* file_name_pointer;
    file_name_pointer = fileName_String(currencyCode);
    
    //printf("파일이름:%s\n", file_name_pointer);


    FILE* fp = fopen(file_name_pointer, "w");
    fputs(jsonData, fp);
    fclose(fp);

    //동적 메모리로 지정해 놓아서 값이 남아 있다.
    //파일 이름이 남아있으면 파일이름이 두번 찍히므로
    //파일이름 변수를 초기화 해줘야한다.
    free(file_name_pointer);
}
//은행에서의 수수료
void bankFeeCal(double money,char* selectCode) {
    
    //은행 환전수수료율
    //1.75%
    double bankFeeRate = 0.0175;
    //은행이 가져가는 수수료 변수
    double bankFee = 0;

    bankFee = money * bankFeeRate;

    printf("현찰로 구매할시 계산:");
    printf("%f ", money - bankFee);
    printf("%s\n", selectCode);

}

void failRequest(char* currencyCode, char* selectCode,int money) {
    // currencyCode 현재 나의 돈 국가코드
    // selectCode 환전하려는 나라의 돈 국가코드
    // money 현재 나의 돈의 량

    FILE* saveFile;
    char* file_name;
    file_name= fileName_String(currencyCode);
    printf("파일이름:%s\n", file_name);
    saveFile =fopen(file_name, "r");
    if (saveFile!=NULL) {
        //파일 사이즈 변수
        int fileSize=0;
       
        //파일에서 글자를 받아와서 저장할 변수
        char *bufferString;
        
        //환율 비율 변수
        double rate_value;

        //환율 계산 결과값을 저장하기 위한 변수
        double exchang_result = 0;

        //파일끝으로 이동
        fseek(saveFile, 0, SEEK_END);
        //파일사이즈 구하기 위한 함수
        fileSize = ftell(saveFile);
        fseek(saveFile, 0, SEEK_SET);

        //printf("파일크기:%d\n", fileSize);

        bufferString = (char*)malloc(fileSize+20);

        fgets(bufferString, fileSize+10, saveFile);
        
        //printf("파일내용:%s\n", bufferString);
      
        //전체 json 값을 저장하기 위한 변수
        struct json_object* jobj= json_tokener_parse(bufferString);

        //전체 json 값중에서 환율 비율만 저장하기 위한 변수
        struct json_object* rate_obj;

        //선택한 국가의 환율 비율만 저장하기 위한 변수
        struct json_object* select_rate;

        json_object_object_get_ex(jobj, "rates", &rate_obj);
        
        json_object_object_get_ex(rate_obj, selectCode, &select_rate);

        rate_value = json_object_get_double(select_rate);

        exchang_result = money * rate_value;

        printf("매매기준 환전 계산결과: %f ", exchang_result);
        printf("%s\n",selectCode);

        bankFeeCal(exchang_result, selectCode);


        json_object_put(jobj);
        free(bufferString);
        free(file_name);
        fclose(saveFile);
    }
    else {
        printf("인터넷 연결에 문제가 발생했습니다.\n");
        printf("인터넷 연결에 문제가 없는지 확인해 주세요.\n");
    }
}


void exchange_trends(double exchang_result ,char* currencyCode,char* selectCode,int money) {
    printf("현재 환전:%f(%s)\n", exchang_result, selectCode);
    
    int currency_con_KRW = strcmp(currencyCode, "KRW");
    int currency_con_USD = strcmp(currencyCode, "USD");
    int currency_con_JPY = strcmp(currencyCode, "JPY");

    int selectCode_con_KRW = strcmp(selectCode, "KRW");
    int selectCode_con_USD = strcmp(selectCode, "USD");
    int selectCode_con_JPY = strcmp(selectCode, "JPY");

    if (currency_con_KRW==0) {
        printf("테스트\n");
        if (selectCode_con_USD ==0) {
            double ex_sum = 0;
            double KRW_EX_USD[5] = { 0.000767, 0.000766,  0.000765,  0.000764,  0.000763 };
            for (int i = 0; i < 5; i++) {
                ex_sum =ex_sum+(money * KRW_EX_USD[i]);
            }
            printf("현재 %s 평균환율:%f\n", selectCode, ex_sum / 5);
            
            if (exchang_result > ex_sum) {
                printf("전일 평균대비 환율상승\n");
            }
            else {
                printf("전일 평균대비 환율하락\n");
            }
        }
        else if (selectCode_con_JPY == 0) {
            double ex_sum = 0;
            double KRW_EX_JPY[5] = { 0.107154, 0.107153,  0.107151,  0.107152,  0.107156 };
            for (int i = 0; i < 5; i++) {
                ex_sum = ex_sum + (money * KRW_EX_JPY[i]);
            }
            printf("현재 %s 평균환율:%f\n", selectCode, ex_sum / 5);

            if (exchang_result > ex_sum) {
                printf("전일 평균대비 환율상승\n");
            }
            else {
                printf("전일 평균대비 환율하락\n");
            }
        }
        
    }else if (currency_con_USD == 0) {

        if (selectCode_con_KRW == 0) {
            double ex_sum = 0;
            double USD_EX_KRW[5] = { 1302.580201, 1311.580201,1303.580201, 1308.580201, 1291.580201 };
            for (int i = 0; i < 5; i++) {
                ex_sum = ex_sum + (money * USD_EX_KRW[i]);
            }
            printf("현재 %s 평균환율:%f\n", selectCode, ex_sum / 5);

            if (exchang_result > ex_sum) {
                printf("전일 평균대비 환율상승\n");
            }
            else {
                printf("전일 평균대비 환율하락\n");
            }
        }
        else if (selectCode_con_JPY == 0) {
            double ex_sum = 0;
            double USD_EX_JPY[5] = { 9.322149, 9.322148,  9.322145, 9.3221473,  9.322143 };
            for (int i = 0; i < 5; i++) {
                ex_sum = ex_sum + (money * USD_EX_JPY[i]);
            }
            printf("현재 %s 평균환율:%f\n", selectCode, ex_sum / 5);

            if (exchang_result > ex_sum) {
                printf("전일 평균대비 환율상승\n");
            }
            else {
                printf("전일 평균대비 환율하락\n");
            }
        }

    }else if (currency_con_JPY == 0) {
        if (selectCode_con_KRW == 0) {
            double ex_sum = 0;
            double JPY_EX_KRW[5] = { 9.322149, 9.322148,  9.322145, 9.322147,  9.322143 };
            for (int i = 0; i < 5; i++) {
                ex_sum = ex_sum + (money * JPY_EX_KRW[i]);
            }
            printf("현재 %s 평균환율:%f\n", selectCode, ex_sum / 5);

            if (exchang_result > ex_sum) {
                printf("전일 평균대비 환율상승\n");
            }
            else {
                printf("전일 평균대비 환율하락\n");
            }
        }
        else if (selectCode_con_USD == 0) {
            double ex_sum = 0;
            double JPY_EX_USD[5] = { 0.007159, 0.007154,  0.007153,  0.007153,  0.007152 };
            for (int i = 0; i < 5; i++) {
                ex_sum = ex_sum + (money * JPY_EX_USD[i]);
            }
            printf("현재 %s 평균환율:%f\n", selectCode, ex_sum / 5);

            if (exchang_result > ex_sum) {
                printf("전일 평균대비 환율상승\n");
            }
            else {
                printf("전일 평균대비 환율하락\n");
            }
        }
    }
}

//환율 api 호출해서 값 불러오고 가져와서 계산하는 함수
void exchange_api_request(char *currencyCode,char* selectCode,int money){
    //printf("%s\n", currencyCode);

    CURL* curl;
    CURLcode res;

    //요청 서버 url
    char *url = malloc(sizeof(char) * 80);

    strcpy(url, baseUrl);

    strcat(url, currencyCode);

    //printf("baseurl:%s\n", url);
    
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

            failRequest(currencyCode,selectCode, money);

            //요청 실패시 실행
            //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            
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
            
            save_jobjFile(save_data_string, currencyCode);

            printf("환율 비율: %f\n", json_object_get_double(rate_obj));

            //환율비율
            double rate_value = json_object_get_double(rate_obj);

            //환율 계산 결과
            double exchang_result = rate_value * money;

            printf("매매기준 환전 계산결과: %f ", exchang_result);
            printf("%s\n", selectCode);
            bankFeeCal(exchang_result, selectCode);
            exchange_trends(exchang_result, currencyCode, selectCode, money);

            json_object_put(jobj);
        }
    }
    free(url);
    free(response_buf);
}


int main() {

    char* select_Code_Value;
    char* target_Code_Value;
    int money=0;

    //국가코드 구조체로 설정
    struct CurrencyCode currencyCode;

    //구조체를 초기화 하는 함수를 실행해서
    //currencyCode 구조체에 값을 넣는다.
    currencyCode = initStruct();


    while (1) {
        //나의 돈 나라 코드
        select_Code_Value = selectCode();

        //바꾸려는 돈의 나라 코드
        target_Code_Value = targetSelectCode();

        printf("돈입력(%s):",select_Code_Value);
        scanf("%d", &money);

        //KRW 베이스 돈,USD 환전하려는 나라의 돈,5000 얼마나 환전할건지
        exchange_api_request(select_Code_Value, target_Code_Value, money);
    }

    system("pause > nul");
	return 0;
}