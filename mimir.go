package main

import (
	"bytes"
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
)

var (
	status map[string]string
)

// ArduinoPayload payload sized for arduino
type ArduinoPayload struct {
	Target string `json:"target"`
	Status string `json:"status"`
}

// Alertmanager holds data coming from the Alertmanager webhook
type Alertmanager struct {
	Status string  `json:"status"`
	Alerts []Alert `json:"alerts"`
}

// Alert holds a single alert
type Alert struct {
	Labels interface{} `json:"labels"`
}

// JSONReq holds the data from a generic JSON request
type JSONReq struct {
	Target string `json:"target"`
	Status string `json:"status"`
}

func main() {
	status = make(map[string]string)
	http.HandleFunc("/alertmanager", HandleAlertmanager)
	http.HandleFunc("/json", HandleJSON)
	log.Fatal(http.ListenAndServe(":18081", nil))
}

// Generic JSON section

// HandleJSON handles incoming JSON data that has at least following fields
// {
//	target: the device that has gone down
//	status: status should be ok, warning or critical
// }
func HandleJSON(w http.ResponseWriter, req *http.Request) {
	var js JSONReq
	json.NewDecoder(req.Body).Decode(&js)
	req.Body.Close()
	JSONUpdate(&js, &status)
	return
}

// JSONUpdate update the status map with new data from generic JSON
// Push to microcontrollers afterwards
func JSONUpdate(js *JSONReq, m *map[string]string) {
	log.Print("Updating map ...")
	(*m)[js.Target] = js.Status
	log.Print("Map updated:")
	log.Println(*m)
	SendArduinoMap(&status)
}

// Alertmanager section

// HandleAlertmanager handles incoming requests from Alertmanager
func HandleAlertmanager(w http.ResponseWriter, req *http.Request) {
	var alertm Alertmanager
	json.NewDecoder(req.Body).Decode(&alertm)
	req.Body.Close()
	AlertmanagerUpdate(&alertm, &status)
	return
}

// AlertmanagerUpdate update the status map with new data from Alertmanager
// Push to microcontrollers afterwards
func AlertmanagerUpdate(alertm *Alertmanager, m *map[string]string) {
	//targetStat := "10.3.21.14"
	log.Print("Updating map ...")
	for i := range alertm.Alerts {
		alert := alertm.Alerts[i]
		target := (alert.Labels.(map[string]interface{})["instance"]).(string)
		if alertm.Status == "resolved" {
			(*m)[target] = "ok"
		} else {
			(*m)[target] = (alert.Labels.(map[string]interface{})["severity"]).(string)
		}
	}
	log.Print("Map updated:")
	log.Println(*m)
	SendArduinoMap(&status)
}

// Transmission parts

// SendArduinoMap process the targets in the map and send the data to the arduinos
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

	log.Println("response Status:", resp.Status)
	log.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	log.Println("response Body:", string(body))
}
