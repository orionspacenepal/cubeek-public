

/************************ declering variables*********************************/

const xlabels = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const yalt = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const ytemp = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const yldrvalue = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const ypressure = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const yroll = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const ypitch = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
const yyaw = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];


/************************* CHART FOR ALTITUDE ******************************/

const ctxA = document.getElementById("chart-alt").getContext("2d");
const myCharta = new Chart(ctxA, {
  type: "line",
  data: {
    labels: xlabels,
    datasets: [
      {
        label: "ALTITUDE",
        data: yalt,
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
});

/************************* CHART FOR INTENSITY ******************************/

/************************* CHART FOR TEMPERATURE ******************************/

const ctx1 = document.getElementById("chart-1").getContext("2d");
const myChart1 = new Chart(ctx1, {
  type: "line",
  data: {
    labels: xlabels,
    datasets: [
      {
        label: "TEMPERATURE",
        data: ytemp,
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
});

/************************* CHART FOR INTENSITY ******************************/

const ctx2 = document.getElementById("chart-2").getContext("2d");
const myChart2 = new Chart(ctx2, {
  type: "line",
  data: {
    labels: xlabels,
    datasets: [
      {
        label: "INTENSITY",
        data: yldrvalue,
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
});

/************************* CHART FOR PRESSURE ******************************/

const ctx3 = document.getElementById("chart-3").getContext("2d");
const myChart3 = new Chart(ctx3, {
  type: "line",
  data: {
    labels: xlabels,
    datasets: [
      {
        label: "PRESSURE",
        data: ypressure,
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
});
/************************* CHART FOR ROLL ******************************/

const ctx4 = document.getElementById("chart-4").getContext("2d");
const myChart4 = new Chart(ctx4, {
  type: "line",
  data: {
    labels: xlabels,
    datasets: [
      {
        label: "ROLL",
        data: yroll,
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
});

/************************* CHART FOR PITCH ******************************/
const ctx5 = document.getElementById("chart-5").getContext("2d");
const myChart5 = new Chart(ctx5, {
  type: "line",
  data: {
    labels: xlabels,
    datasets: [
      {
        label: "PITCH",
        data: ypitch,
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
});
/************************* CHART FOR YAW ******************************/

const ctx6 = document.getElementById("chart-6").getContext("2d");
const myChart6 = new Chart(ctx6, {
  type: "line",
  data: {
    labels: xlabels,
    datasets: [
      {
        label: "YAW",
        data: yyaw,
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
});


//builds socket connection with server to fetch dataarray and send port-number

var socket = io.connect("http://localhost:4000");

// fetch dataarray and stores in object message
socket.on("dataArray", function (message) {
  //calculate current time for x-axis
  var today = new Date();
  var time =
    today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
  xlabels.push(time); //push current time in xlabel(x-axis data)
  ytemp.push(message.dataArray[0]); //dataArray[0] contains temp data
  yldrvalue.push(message.dataArray[1]); //dataArray[1] contains ldr data
  ypressure.push(message.dataArray[2]); //dataArray[2] contains pressure data
  yroll.push(message.dataArray[3]); //dataArray[3] contains roll data
  ypitch.push(message.dataArray[4]); //dataArray[4] contains pitch data
  yyaw.push(message.dataArray[5]); //dataArray[5] contains yaw data
  yalt.push(message.dataArray[6]);//dataArray[5] contains altitude data
  

  /********************* shifting the dataarray *********************/
  xlabels.shift();
  ytemp.shift();
  yldrvalue.shift();
  ypressure.shift();
  yroll.shift();
  ypitch.shift();
  yyaw.shift();
  yalt.shift();

  /*********************** updating the chart ***********************/
  myChart1.update();
  myChart2.update();
  myChart3.update();
  myChart4.update();
  myChart5.update();
  myChart6.update();
  myCharta.update();
 
});