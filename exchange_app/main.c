#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <string.h>    // strcat �Լ��� ����� ��� ����
#include <stdlib.h>    // malloc, free �Լ��� ����� ��� ����


char* baseUrl = "https://nginx-nginx-r8xoo2mleehqmty.sel3.cloudtype.app/rate/";

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t realsize = size * nmemb;
    char* data = (char*)userdata;
    /* ���� �����͸� userdata�� �߰� */
    memcpy(data, ptr, realsize);

    return realsize;
}

void exchange_api_request(char *currencyCode,char* selectCode,int money){
    printf("%s\n", currencyCode);
    CURL* curl;
    CURLcode res;

    //��û ���� url
    char *url = malloc(sizeof(char) * 80);

    strcpy(url, baseUrl);

    strcat(url, currencyCode);

    


    printf("baseurl:%s\n", url);
    
    //��û ����� �����ϱ����� ���� ũ�� ����
    char* response_buf = (char*)malloc(2048 * 2048);


    //��� ����
    struct curl_slist* headers = NULL;

    //���� json ������
    //struct json_object* reponse_msg;
    
    //��� ����
    headers = curl_slist_append(headers, "Content-Type: application/json");

    //curl ��ü �ʱ�ȭ
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl) {
        //api url ����
        curl_easy_setopt(curl, CURLOPT_URL, url);

        //curl ��� ���� ����
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        //��û�� respons �� ó�� ������ �����ϴ� �ڵ�
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        //respons �� ���ۿ� �����ϱ� ���� �ڵ�
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response_buf);

        //��û �����ϴ� �ڵ�
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            
            printf("��û����\n");

            //��û ���н� ����
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            
        }
        else {
            //��û ����
            
            printf("��û����\n");

            //��û ������ ���� json�� ���� �������� ���� �ڵ�
            struct json_object* jobj = json_tokener_parse(response_buf);
            
            //ȯ�� ���� json �� �������� ���� ����ü
            struct json_object* exchang_rate_obj;

            //��ü ȯ�� ������ �������� ���� ����ü {ex(USD,JPY,KRW,etc)}
            struct json_object* all_rate_obj;

            //Ư�� ȯ�� ������ �������� ���� ����ü
            struct json_object* rate_obj;



            json_object_object_get_ex(jobj, "rate_data", &exchang_rate_obj);

            json_object_object_get_ex(exchang_rate_obj, "rates",&all_rate_obj);
            
            json_object_object_get_ex(all_rate_obj, selectCode, &rate_obj);

            printf("ȯ�� ����: %f\n", json_object_get_double(rate_obj));

            //ȯ������
            double rate_value = json_object_get_double(rate_obj);

            //ȯ�� ��� ���
            double exchang_result = rate_value * money;

            printf("�����: %f\n", exchang_result);
            

        }
    }

    free(url);
    free(response_buf);


}


int main() {
    //KRW ���̽� ��,USD ȯ���Ϸ��� ������ ��,5000 �󸶳� ȯ���Ұ���
    exchange_api_request("KRW","USD",5000);
    system("pause > nul");
	return 0;
}