#include <iostream>
#include <fstream>
#include "json.hpp"
#include <curl/curl.h>

#define reset_stream(ss) \
  ss.str(""); \
  ss.clear(std::stringstream::goodbit);

size_t
write_cb(char *ptr, size_t size, size_t nmemb, std::string *strm) {
  size_t len = size * nmemb;
  strm->append(ptr, len);
  return len;
}

int
main(int argc, char* argv[]) {
  if (argc < 3) {
	std::cerr << "usage: " << argv[0] << " [compiler] [file] [arguments...]" << std::endl;
    return 1;
  }
  std::stringstream ss;
  if (std::string(argv[2]) != "-") {
    std::ifstream in(argv[2], std::ifstream::in);
    std::string line;
    std::getline(in, line);
    if (line.find("#!") != 0) ss << line + "\n";
    ss << in.rdbuf();
  } else {
    ss << std::cin.rdbuf();
  }

  nlohmann::json obj;
  obj["code"] = ss.str();

  reset_stream(ss);
  obj["compiler"] = argv[1];
  for (int i = 3; i < argc; i++) {
    if (i > 3)  ss << "\n";
    ss << argv[i];
  }
  obj["runtime-option-raw"] = ss.str();

  reset_stream(ss);
  ss << obj;

  std::string chunk;

  CURLcode ret;
  CURL *curl = curl_easy_init();
  if (curl == NULL) {
	std::cerr << "curl_easy_init() failed" << std::endl;
	return 1;
  }
  std::string data = ss.str();

  struct curl_slist *headerlist = NULL;
  headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
  curl_easy_setopt(curl, CURLOPT_URL, "http://melpon.org/wandbox/api/compile.json");
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
  ret = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  curl_slist_free_all(headerlist);

  if (ret != CURLE_OK) {
	std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(ret) << std::endl;
	return 1;
  }

  obj = nlohmann::json::parse(chunk);
  try {
	std::cout << obj["program_message"].get<std::string>();
  } catch(std::exception& e) {
	std::cerr << obj["compiler_message"].get<std::string>() << std::endl;
  }

  int code;
  reset_stream(ss);
  ss << obj["status"];
  ss >> code;
  return code;
}
