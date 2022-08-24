/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <unistd.h>
#include <ulfius.h>
#include <jansson.h>
#include <string.h>

#include "http_compression_callback.h"
#include "static_compressed_inmemory_website_callback.h"
#include "u_example.h"
//#include "routes.h"

#define PORT 8080
#define STATIC_PREFIX "static"
#define FILE_PREFIX "/upload"

/**
 * decode a u_map into a string
 */
char * print_map(const struct _u_map * map) {
  char * line, * to_return = NULL;
  const char **keys, * value;
  int len, i;
  if (map != NULL) {
    keys = u_map_enum_keys(map);
    for (i=0; keys[i] != NULL; i++) {
      value = u_map_get(map, keys[i]);
      len = snprintf(NULL, 0, "key is %s, length is %zu, value is %.*s", keys[i], u_map_get_length(map, keys[i]), (int)u_map_get_length(map, keys[i]), value);
      line = o_malloc((len+1)*sizeof(char));
      snprintf(line, (len+1), "key is %s, length is %zu, value is %.*s", keys[i], u_map_get_length(map, keys[i]), (int)u_map_get_length(map, keys[i]), value);
      if (to_return != NULL) {
        len = o_strlen(to_return) + o_strlen(line) + 1;
        to_return = o_realloc(to_return, (len+1)*sizeof(char));
        if (o_strlen(to_return) > 0) {
          strcat(to_return, "\n");
        }
      } else {
        to_return = o_malloc((o_strlen(line) + 1)*sizeof(char));
        to_return[0] = 0;
      }
      strcat(to_return, line);
      o_free(line);
    }
    return to_return;
  } else {
    return NULL;
  }
}
 

void update_screen(){
    char *EPD_BINARY_PATH = getenv("EPD_BINARY_PATH");
    char epd_executable[100], image_path[100];
    //char *image_path = "";

    strcpy(epd_executable, EPD_BINARY_PATH);
    strcpy(image_path, EPD_BINARY_PATH);
    
    strcat(epd_executable, "/epd");
    strcat(image_path, "/pic/800x600_3.bmp");
    pid_t pid; 
    pid = fork();
    
    y_log_message(Y_LOG_LEVEL_DEBUG, "Executing %s with pic %s", epd_executable, image_path);
    // Child
    if (pid == 0){
        // Execute 
        char *args[] = {epd_executable, "-1.5", "3", image_path, NULL};
        int status_code = execv(epd_executable, args);
        y_log_message(Y_LOG_LEVEL_DEBUG, "Executed Fork from child with status code %d", status_code );
	exit(-1);
    } else if (pid == -1){
       y_log_message(Y_LOG_LEVEL_DEBUG, "Issue creating fork");
    } else {
       y_log_message(Y_LOG_LEVEL_DEBUG, "Logging from Parent Process");
    }

  
}

/**
 * Callback for file upload
 */
static int callback_upload_file (const struct _u_request * request, struct _u_response * response, void * user_data) {
  char * url_params = print_map(request->map_url), * headers = print_map(request->map_header), * cookies = print_map(request->map_cookie), 
       * post_params = print_map(request->map_post_body);

  char * string_body = msprintf("Upload file\n\n  method is %s\n  url is %s\n\n  parameters from the url are \n%s\n\n  cookies are \n%s\n\n  headers are \n%s\n\n  post parameters are \n%s\n\n",
                                  request->http_verb, request->http_url, url_params, cookies, headers, post_params);
  y_log_message(Y_LOG_LEVEL_DEBUG, "Post parameters:\n%s", post_params);
  y_log_message(Y_LOG_LEVEL_DEBUG, "URL parameters:\n%s", url_params);
  o_free(url_params);
  o_free(headers);
  o_free(cookies);
  o_free(post_params);
  o_free(string_body);

  const char * param = "upload_file";

  const char * file_data = u_map_get(request->map_post_body, param);
  size_t file_size = u_map_get_length(request->map_post_body, param);

  y_log_message(Y_LOG_LEVEL_DEBUG, "FILE DATA %s", file_data);
  FILE *write_ptr; 
  write_ptr = fopen("./static/test.bin", "wb");

  fwrite(file_data, file_size, 1, write_ptr);
  fclose(write_ptr);

  update_screen();

  return U_CALLBACK_CONTINUE;
}



/**
 * main function
 */
int main(int argc, char **argv) {
    struct _u_instance instance;
    struct _u_compressed_inmemory_website_config file_config;

    y_init_logs("Webservice", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting webservice")    ;

  if (u_init_compressed_inmemory_website_config(&file_config) == U_OK) {
    u_map_put(&file_config.mime_types, ".html", "text/html");
    u_map_put(&file_config.mime_types, ".css", "text/css");
    u_map_put(&file_config.mime_types, ".js", "application/javascript");
    u_map_put(&file_config.mime_types, ".png", "image/png");
    u_map_put(&file_config.mime_types, ".jpg", "image/jpeg");
    u_map_put(&file_config.mime_types, ".jpeg", "image/jpeg");
    u_map_put(&file_config.mime_types, ".ttf", "font/ttf");
    u_map_put(&file_config.mime_types, ".woff", "font/woff");
    u_map_put(&file_config.mime_types, ".woff2", "font/woff2");
    u_map_put(&file_config.mime_types, ".map", "application/octet-stream");
    u_map_put(&file_config.mime_types, ".json", "application/json");
    u_map_put(&file_config.mime_types, "*", "application/octet-stream");
    file_config.files_path = "static";
    file_config.url_prefix = FILE_PREFIX;

    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error ulfius_init_instance, abort");
      return(1);
    }

    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
      fprintf(stderr, "Error ulfius_init_instance, abort\n");
      return(1);
    }

    // Upload related configurations, max file size 1 MB
    instance.max_post_param_size = 1024*1024;
    instance.check_utf8 = 0;

    ulfius_add_endpoint_by_val(&instance, "POST", STATIC_PREFIX, "/upload", 1, &callback_upload_file, NULL);

    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);

    // Wait for the user to press <enter> on the console to quit the application
    getchar();
    } else {
    fprintf(stderr, "Error starting framework\n");
    }

    printf("End framework\n");
    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);
    u_clean_compressed_inmemory_website_config(&file_config);

    return 0;
    }

    y_close_logs();
    return 0;

}
