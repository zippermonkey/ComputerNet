#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

//�����ļ����Ͷ�Ӧ��content-tyoe
struct doc_type {
  char *suffix;
  char *type;
};

//�ļ�
struct doc_type file_type[] = {{"html", "text/html"},
                               {"gif", "imag/gif"},
                               {"jpeg", "imag/jpeg"},
                               {"jpg", "image/jpeg"},
                               {"ico", "image/vnd.microsoft.icon"},
                               {NULL, NULL}};

//��Ӧ�ײ�����
char *http_res_hdr_tmp1 =
    "HTTP/1.1 200 OK \r\nServer:Zippermonkey's Server<0.1>\r\n"
    "Accept-Ranges:bytes\r\nContent-Length:%d\r\nConnection:close\r\n"
    "Content-Type:%s\r\n\r\n";
char *not_found_head = "HTTP/1.1 404 NOT FOUND\r\n\r\n";

//ͨ����׺�����ҵ���Ӧ��content-type
char *http_get_type_by_suffix(const char *suffix) {
  struct doc_type *type;
  for (type = file_type; type->suffix; type++) {
    if (strcmp(type->suffix, suffix) == 0)
      return type->type;
  }
  return NULL;
}

//�����ͻ��˷��͹���������
void http_parse_request_cmd(char *buf, int buflen, char *file_name,
                            char *suffix) {
  printf("%s\n", buf); // ��ӡ������
  int length = 0;
  char *begin, *end, *bias;

  //����URL��ʼλ��
  begin = strchr(buf, ' ');
  begin++;

  //����URL����λ��
  end = strchr(begin, ' ');
  *end = 0;

  bias = strrchr(begin, '/');
  length = end - bias;

  //�ҵ��ļ�����ʼ��λ��
  if ((*bias == '/') || (*bias == '\\')) {
    bias++;
    length--;
  }

  //�õ��ͻ���������ļ���
  if (length > 0) {
    memcpy(file_name, bias, length);
    file_name[length] = 0;

    begin = strchr(file_name, '.');
    if (begin)
      strcpy(suffix, begin + 1);
  }
}

int http_send_response(SOCKET soc, char *buf, int buf_len) {
  int read_len, file_len, hdr_len, send_len;
  char *type;
  char read_buf[1024];
  char http_header[1024];
  char file_name[256] = "index.html";
  char suffix[16] = "html";
  FILE *res_file;

  //ͨ������URL���õ��ļ���
  http_parse_request_cmd(buf, buf_len, file_name, suffix);

  //���ļ�
  res_file = fopen(file_name, "rb+");

  if (res_file == NULL) {
    printf("[Web]�ļ�:%s ������!\n", file_name);
    // ����404
    hdr_len = strlen(not_found_head);
    send_len = send(soc, not_found_head, hdr_len, 0);
    if (send_len == SOCKET_ERROR) {
      fclose(res_file);
      printf("[Web]����ʧ�ܣ�����:%d\n", WSAGetLastError());
      return 0;
    }
    return 0;
  }

  //�����ļ���С
  fseek(res_file, 0, SEEK_END);
  file_len = ftell(res_file);
  fseek(res_file, 0, SEEK_SET);

  //����ļ�content-type
  type = http_get_type_by_suffix(suffix);

  if (type == NULL) {
    printf("[Web]û����ص��ļ�����!\n");
    return 0;
  }

  //������Ӧ�ײ��������ļ����ȣ�content-type��Ϣ
  hdr_len = sprintf(http_header, http_res_hdr_tmp1, file_len, type);
  send_len = send(soc, http_header, hdr_len, 0);

  if (send_len == SOCKET_ERROR) {
    fclose(res_file);
    printf("[Web]����ʧ�ܣ�����:%d\n", WSAGetLastError());
    return 0;
  }

  //�����ļ�
  do {
    read_len = fread(read_buf, sizeof(char), 1024, res_file);
    if (read_len > 0) {
      send_len = send(soc, read_buf, read_len, 0);
      file_len -= read_len;
    }
  } while ((read_len > 0) && (file_len > 0));
  fclose(res_file);
  return 1;
}
int main() {
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
  SOCKET serversoc, acceptsoc;
  SOCKADDR_IN serveraddr;
  SOCKADDR_IN fromaddr;
  char Recv_buf[1024];
  int from_len = sizeof(fromaddr);
  int result;
  int Recv_len;

  //����socket
  serversoc = socket(AF_INET, SOCK_STREAM, 0);
  if (serversoc == INVALID_SOCKET) {
    printf("[Web]�����׽���ʧ��!");
    return -1;
  }

  //��ʼ��������IP,Port
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(8080);
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //��socket
  result = bind(serversoc, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
  if (result == SOCKET_ERROR) {
    closesocket(serversoc);
    printf("[Web]���׽���ʧ��!");
    return -1;
  }

  //����socket����
  result = listen(serversoc, 3);
  printf("[Web]��������������.....\n");

  while (1) {
    //��������
    acceptsoc = accept(serversoc, (SOCKADDR *)&fromaddr, &from_len);
    if (acceptsoc == INVALID_SOCKET) {
      printf("[Web]��������ʧ��!");
      break;
    }
    printf("[Web]�������� IP:  %s  Port:  %d  \n", inet_ntoa(fromaddr.sin_addr),
           ntohs(fromaddr.sin_port));

    //�������Կͻ��˵�����
    Recv_len = recv(acceptsoc, Recv_buf, 1024, 0);
    if (Recv_len == SOCKET_ERROR) {
      printf("[Web]��������ʧ��!");
      break;
    }
    Recv_buf[Recv_len] = 0;
    //��ͻ��˷�����Ӧ����
    result = http_send_response(acceptsoc, Recv_buf, Recv_len);

    closesocket(acceptsoc);
  }

  closesocket(serversoc);
  WSACleanup();

  return 0;
}