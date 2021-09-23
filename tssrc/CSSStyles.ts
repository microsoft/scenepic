var css = `
.svt {
	font-family: Calibri,Tahoma,Verdana,Segoe,sans-serif;
	font-size: 14px;
}

.svt-textbox-header {
    vertical-align:top;
    margin:1px;
    padding:1px;
    border: 2px solid white;
    font-weight: bold;
    display: inline-block;
}

.svt-textbox-content {
    margin:1px;
    vertical-align:top;
    padding:1px;
    border: 2px solid #fafafa;
    display: inline-block;
}

.svt-status-bar {
    position:fixed;
    bottom:0;
    marginBottom: 30;
    width: 100%;
    background-color: white;
}

.svt-canvas-container {
    border: 2px solid lightgrey;
    display: inline-block;
    vertical-align: top;
    position: relative;
}

.svt-canvas {
    touch-action: none;
    display: block;
    outline-style: none;
}

.svt-slider-container {
  position: absolute;
  background: grey;
  opacity: 0.5;
  top: 5px;
  left: 5px;
  display: flex;
}

.svt-slider {
  margin: 5px;
  vertical-align: middle;
  min-width: 100px;
}

.svt-slider-text {
  width: auto;
  height: auto;
  color: white;
  vertical-align: middle;
  display: inline-block;
  font-family: monospace;
}

.svt-dropdown {
  background: grey;
  opacity: 0.75;
  width: 21px;
  height: 21px;
  box-sizing: content-box;
  cursor: pointer;
  padding: 5px;
  position: absolute;
  margin: 0 auto;
  top: 5px;
  right: 5px;
  user-select: none;
}

.svt-dropdown:after {
  content: '';
  height: 0;
  position: absolute;
  width: 0;
  border: 6px solid transparent;
  border-top-color: #000;
  top: 50%;
  right: 10px;
  margin-top: -3px;
}

.svt-dropdown.is-active:after {
  border-bottom-color: #000;
  border-top-color: transparent;
  margin-top: -9px;
}

.svt-dropdown-table {
  list-style: none !important;
  margin: 0 !important;
  padding: 0;
  position: absolute;
  top: 100%;
  border: inherit;
  right: -1px;
  visibility: hidden;
  background-color: white;
  pointer-events: none;
  border-collapse: collapse;
}

.is-active .svt-dropdown-table {
  visibility: visible;
  pointer-events: auto;
}

.svt-dropdown-header {
  text-align:center;
  padding:3px;
  white-space: nowrap;
}

.svt-table-control {
  text-align:center;
}

.svt-table-layerid {
  padding-left:5px;
  padding-right:5px;
  white-space: nowrap;
}

.svt-progress {
  height: 12px;
  width: 400px;
  position: relative;
  background: #aaa;
  -moz-border-radius: 7px;
  -webkit-border-radius: 7px;
  border-radius: 7px;
  padding: 3px;
  margin: 5px;
  box-shadow: inset 0 -1px 1px rgba(255,255,255,0.3);
}

.svt-progress > span {
  display: block;
  height: 100%;
  border-top-right-radius: 8px;
  border-bottom-right-radius: 8px;
  border-top-left-radius: 20px;
  border-bottom-left-radius: 20px;
  background-color: rgb(43,194,83);
  background-image: linear-gradient(
    center bottom,
    rgb(43,194,83) 37%,
    rgb(84,240,84) 69%
  );
  box-shadow: 
    inset 0 2px 9px  rgba(255,255,255,0.3),
    inset 0 -2px 6px rgba(0,0,0,0.4);
  position: relative;
  overflow: hidden;
}

@keyframes blink {
  0% { opacity: 0; }
  50% { opacity: 1; }
  100% { opacity: 0; }
}

.is-blinking {
  animation: blink 2s infinite;
}

.svt-controls {
  padding: 5px;
  background: #ddd;
  border-radius: 10px;
  width: 330px;  
}

.svt-toggle-button {
  position: relative;
  display: inline-block;
  width: 22px;
  height: 22px;
  vertical-align: middle;
  border: 1px solid #ddd;
  border-radius: 20px;
}

.svt-toggle-button:hover {
  border: 1px solid gray;
}

.svt-toggle-button:active {
  background-color: gray;
}

/* Hide default HTML checkbox */
.svt-toggle-button input {
  opacity: 0;
  width: 0;
  height: 0;
  padding: 0;
  margin: 0;
}

/* The icon */
.svt-toggle-button span {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
}

label.svt-large {
  width: 42px;
  height: 42px;
}

.svt-controls button.svt-medium {
  width: 32px;
  height: 32px;
}

.svt-icon-playpause {
  width: 38px;
  height: 38px;
  box-sizing: content-box;
  border: 2px solid black;
  border-radius: 20px;
  background: url(data:image/gif;base64,R0lGODlhIAAgAIQcAAEBAQMDAwYGBggICAoKCgsLCw0NDQ8PDxISEhQUFBUVFRYWFhkZGRsbGx0dHR8fHyAgICIiIiQkJCUlJScnJykpKSsrKywsLC4uLjAwMDExMTMzM7u7u7u7u7u7u7u7uyH+EUNyZWF0ZWQgd2l0aCBHSU1QACH5BAEKAB8ALAAAAAAgACAAAAWP4CeOZGmeaKqubOu+7rbBrmZn9IrtfI5ewODF4itVjshkUURpOp+U5WRKrU4lvoh2y+3SIOCweAx5vBzotHqddjXe8Lg83mLY7/g8vrXo+/+Afi4JhIWGhwkKLwiMjY6PNAeSk5STPgaYmZoGSwSen58FSx8DpaaloyICq6ypIwGwAa4kAACzt7i5uru8RSEAOw==)
              no-repeat 5px 3px;
}

input:checked + .svt-icon-playpause {
  background: url(data:image/gif;base64,R0lGODlhIAAgAIQUAAICAgQEBAcHBwkJCQwMDA4ODhERERMTExYWFhkZGRsbGx4eHiAgICMjIyUlJSgoKCoqKi0tLTAwMDIyMru7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7uyH+EUNyZWF0ZWQgd2l0aCBHSU1QACH5BAEKAB8ALAAAAAAgACAAAAWu4CeOZGmeaKqubOu+cCzP6GTftonjquT/PhMQqIoYj0YTEqmCOJ9OExSqelivVhMWq3J4v14TGKxqmM9mExqtYrjfbhMcrlrY73YTHq9S+P9+JoCAKgmGh4YmiIgqCI6PjiaQkCoHlpeWJpiYKgaen54moKAqBaanpiaoqCoErq+uJrCwKgO2t7YmuLgqAr6/vibAwCoBxsfGJsjIKgDOz84m0NA01dbX2Nna2yghADs=)
              no-repeat 2px 3px;
}

.svt-icon-soundmute {
  background: url(data:image/gif;base64,R0lGODlhEAAQAIQVAAICAgYGBgkJCQwMDA0NDQ8PDxAQEBMTExQUFBcXFxoaGh0dHR4eHiAgICEhISQkJCcnJygoKCsrKy8vLzIyMru7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7uyH+EUNyZWF0ZWQgd2l0aCBHSU1QACH5BAEKAB8ALAAAAAAQABAAAAVR4CeOJEmV6DilqcR+EBm9zzPabOM4Il8ywCDjM1wsSIqk8qMQJUmJaPSTGE1HhyzicyBlSYbwx/ApjMQowmegXrdLApEg/pmjAiQ8C/Dq+z8hADs=)
              no-repeat 3px 3px;
}

input:checked + .svt-icon-soundmute {
  background: url(data:image/gif;base64,R0lGODlhEAAQAIQUAAICAgUFBQYGBgkJCQwMDA0NDQ8PDxAQEBMTExcXFxoaGh4eHiAgICEhISQkJCcnJygoKCsrKy8vLzExMbu7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7uyH+EUNyZWF0ZWQgd2l0aCBHSU1QACH5BAEKAB8ALAAAAAAQABAAAAVb4CeOZGme03lKrBqVUWw+kPg8IlSTTi/2jp+jQSw2Rg2GiLhoOhejpqipqFoVo6qomuh6E6OuqIsomxGjskg9OrhFhvdHPioURHYR4W4alAaAKiQBAgKCJQAjIQA7)
              no-repeat 3px 3px;
}

input.svt-volume-slider {
  display: inline;
  width: 125px;
  vertical-align: middle;
}

.svt-controls button {
  border: none;
  width: 24px;
  height: 24px;
  vertical-align: middle;
  cursor: pointer;
  border: 1px solid #ddd;
  border-radius: 20px;
}

.svt-controls button:focus {
  border: none;
  outline: none;
}

.svt-controls button:hover {
  border: 1px solid gray;
}

.svt-controls button:active {
  background-color: gray;
}

.svt-icon-faster {
  background: url(data:image/gif;base64,R0lGODlhEAAQAIQUAAICAgMDAwYGBgkJCQ0NDRAQEBMTExcXFxoaGh4eHiAgICEhISQkJCcnJygoKCsrKy4uLi8vLzExMTIyMru7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7uyH+EUNyZWF0ZWQgd2l0aCBHSU1QACH5BAEKAB8ALAAAAAAQABAAAAVb4CeOZGlK0qSipghFcBxB5mPf+EM2Tu+MPkdDxCgaR8ajYsFkjppMxSdBbYmoiQ9ia9VyD+Au+PAxmM+jM7rAbo/a7g9hPh/R66OBfs8fmASAgYFWAQCGhgEkIQA7)
              no-repeat 3px 3px;  
}

.svt-icon-slower {
  background: url(data:image/gif;base64,R0lGODlhEAAQAIQUAAICAgMDAwYGBgkJCQ0NDRAQEBMTExcXFxoaGh4eHiAgICEhISQkJCcnJygoKCsrKy4uLi8vLzExMTIyMru7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7u7uyH+EUNyZWF0ZWQgd2l0aCBHSU1QACH5BAEKAB8ALAAAAAAQABAAAAVX4CeOZGlK0qSipghFcBxB5mPf+EM2Tu//joaIQSwajx/FYslsMhWfhLQlkiY+iCwVqz14t97Dx0Aum8+fgnrNbosI8Lg8ThrY7/iBScDv96kBAIKCASQhADs=)
              no-repeat 3px 3px;  
}

.svt-icon-record {
  background: url(data:image/gif;base64,R0lGODlhIAAgAIQYANoAANwAAN4AAOAAAOIAAOQAAOYAAOgAAOoAAOsAAO0AAO8AAPEAAPMAAPUAAPcAAPgAAPkAAPoAAPsAAPwAAP0AAP4AAP8AALu7u7u7u7u7u7u7u7u7u7u7u7u7u7u7uyH5BAEKAB8ALAAAAAAgACAAAAWZ4CeOZGmeaKqubOu+cCzPrmVd+GXTIlX9wCBlJpkYj0ijJAaJOJ9QKOT1qFqvWKvLwe16v91WY0wum80shnrNbrdZi7h8TqezFPi8fr9nJf6AgYKCLAiGh4iJiSwHjY6PkJAsBpSVlpeXLQWbnJ2enC4EoqOkpaMvA6mqq6wDMQKwsbKyMwG2t7i2PB8Avb69u8HCw8TFxschADs=)
              no-repeat -1px -1px;
  margin-right: 5px;
}

.svt-fps {
  position: absolute;
  visibility: hidden;
  left: 5px;
  top: 40px;
  font-family: monospace;
  color: yellow;
  font-size: 14px;  
}

`;

var initialized = false;

export default function InitializeCSSStyles()
{
    if (initialized) return;
    
    var head = document.getElementsByTagName("head")[0];
    var style = document.createElement("style");
    style.type = "text/css";
    style.appendChild(document.createTextNode(css));
    head.appendChild(style);

    initialized = true;
}
