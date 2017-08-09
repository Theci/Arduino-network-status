package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

const promHost string = "10.3.10.10"

// ArduinoPayload to send to Arduin
type ArduinoPayload struct {
	Target string `json:"target"`
	Status string `json:"status"`
}

// LargePayload to be send to more powerful devices
type LargePayload struct {
	Target string `json:"target"`
	Status string `json:"status"`
}

// ICMPResp catches the respons from Prom about ICMP pings
type ICMPResp struct {
	Host    string `json:"instance"`
	Success string `json:"value[1]"`
}

func main() {
	var status map[string]string
	status = make(map[string]string)
	status["10.3.21.14"] = "critical"
	SendArduinoMap(&status)
}

// PromQuery ...
func PromQuery(host string, query string) http.Response {
	resp, err := http.Get("http://" + host + ":9090/api/v1/query?" + query)
	if err != nil {
		panic(err)
	}
	return *resp
}

// PingStatus ...
func PingStatus(m *map[string]string) map[string]string {
	//resp := PromQuery(promHost, "probe_success")
	return *m
}

// SendArduinoMap ...
func SendArduinoMap(m *map[string]string) {
	for k, v := range *m {
		json := ArduinoPayload{Target: k, Status: v}
		SendArduino(&k, &json)
	}
}

// SendArduino : POST the Arduino payload to the targeted Arduino
func SendArduino(target *string, data *ArduinoPayload) {
	buf := new(bytes.Buffer)
	json.NewEncoder(buf).Encode(*data)
	resp, err := http.Post("http://"+*target+":18080", "application/json", buf)
	if err != nil {
		panic(err)
	}
	defer resp.Body.Close()

	fmt.Println("response Status:", resp.Status)
	fmt.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	fmt.Println("response Body:", string(body))
}

// SendLarge : POST the Large payload to the target
func SendLarge(data *LargePayload) {
	buf := new(bytes.Buffer)
	json.NewEncoder(buf).Encode(*data)
	resp, err := http.Post((*data).Target, "application/json; charset=utf-8", buf)
	if err != nil {
		panic(err)
	}
	defer resp.Body.Close()

	fmt.Println("response Status:", resp.Status)
	fmt.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	fmt.Println("response Body:", string(body))
}
