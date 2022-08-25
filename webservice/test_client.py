import requests
files = {'upload_file': open('/home/billburr/eink/frame-display/webservice/static/cropped.bmp','rb')}
values = {'DB': 'photcat', 'OUT': 'csv', 'SHORT': 'short'}

url = "http://localhost:8080/static/upload"

#file_request = requests.Request("POST", url, files=files, data=values)
#prepared_request = file_request.prepare()
#print("PREPARED ", prepared_request.body)

r = requests.post(url, files=files, data=values)
#r = requests.post('http://localhost:7437/helloworld', json={"key": "value"})
print(r.text)

