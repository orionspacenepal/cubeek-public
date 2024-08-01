console.log("Housekeeping script loaded.");

/************************ declaring variables *********************************/
const xlabels = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const ycurrentmA = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const ypowermw = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const yshuntV = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const ybusV = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const yloadV = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

/************************* CHART INITIALIZATION ******************************/
const ctxA = document.getElementById("chart-1").getContext("2d");
const myChart1 = new Chart(ctxA, createChartConfig("CURRENT [mA]", ycurrentmA));

const ctx1 = document.getElementById("chart-2").getContext("2d");
const myChart2 = new Chart(ctx1, createChartConfig("Power [mW]", ypowermw));

const ctx2 = document.getElementById("chart-3").getContext("2d");
const myChart3 = new Chart(ctx2, createChartConfig("Shunt Voltage [mV]", yshuntV));

const ctx3 = document.getElementById("chart-4").getContext("2d");
const myChart4 = new Chart(ctx3, createChartConfig("BUS VOLTAGE [mV]", ybusV));

const ctx4 = document.getElementById("chart-5").getContext("2d");
const myChart5 = new Chart(ctx4, createChartConfig("LOAD VOLTAGE [mV]", yloadV));

function createChartConfig(label, data) {
  return {
    type: "line",
    data: {
      labels: xlabels,
      datasets: [
        {
          label: label,
          data: data,
          backgroundColor: ["rgba(255, 99, 132, 0.2)"],
          borderColor: ["rgba(255, 99, 132, 1)"],
          borderWidth: 1,
        },
      ],
    },
    options: {
      layout: {
        padding: 20,
      },
    },
  };
}

// builds socket connection with server to fetch dataarray and send port-number
var socket = io.connect("http://localhost:4000");

socket.on("dataArray", function (message) {
  console.log("Received housekeepingData:", message);
  var today = new Date();
  var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
  xlabels.push(time);
  ycurrentmA.push(message.dataArray[7]);
  ypowermw.push(message.dataArray[8]);
  yshuntV.push(message.dataArray[9]);
  ybusV.push(message.dataArray[10]);
  yloadV.push(message.dataArray[11]);

  xlabels.shift();
  ycurrentmA.shift();
  ypowermw.shift();
  yshuntV.shift();
  ybusV.shift();
  yloadV.shift();

  myChart1.update();
  myChart2.update();
  myChart3.update();
  myChart4.update();
  myChart5.update();
});

document.addEventListener("DOMContentLoaded", function() {
  console.log("Document loaded.");
  let selectedPort = sessionStorage.getItem('selectedPort');
  if (selectedPort) {
    console.log("Selected port from session storage:", selectedPort);
    socket.emit("message", selectedPort);
  }
});
