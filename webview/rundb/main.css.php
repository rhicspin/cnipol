span.red {
	color: red;
}

.orange {
	color: orange;
}

.bold {
	font-weight: bold;
}

.notbold {
	font-weight: normal;
}

.blue {
	color: blue;
}

.red {
	color: red;
}

.green {
	color: green;
}

.yellow3 {
	color: #bbbb00;
}

.blue3 {
	color: #0000bb;
}

.center {
	text-align: center;
}

.left {
	text-align: left;
}

.align_l {
	text-align: left !important;
}

.align_r {
	text-align: right;
}

.align_lb {
	text-align: left;
	vertical-align: bottom;
}

.align_cm {
	text-align: center !important;
	vertical-align: middle !important;
}

.align_ct {
	text-align: center;
	vertical-align: top;
}

.left {
	text-align: left;
}

.strike {
	text-decoration: line-through;
}

.feature {
	background-color: pink !important;
}

span.feature {
	background-color: pink;
}

.nowrap {
	white-space: nowrap;
}

.bluPol {
	background-color: #8bf !important;
   padding: 0.2em;
}

.yelPol {
	background-color: #eea !important;
   padding: 0.2em;
}

.agsPol {
	background-color: #f88 !important;
   padding: 0.2em;
}

.bkgGreen {
	background-color: #0f0 !important;
   padding: 0.2em;
}

.bkgRed {
	background-color: #f00 !important;
   padding: 0.2em;
}

.frgGreen {
	color: #0f0 !important;
}

.frgRed {
	color: #f00 !important;
}

.bkg1 {
	background-color: #0000ff;
}

.s50      {font-size: 50%;}
.s80      {font-size: 80%;}
.s120      {font-size: 120%;}
.s150      {font-size: 150%;}
.s200      {font-size: 200%;}

.my-th {
	font-weight: bold;
   font-size: 150%;
}

.my-h2 {
	font-weight: bold;
   font-size: 150%;
}

dt.feature {
	font-weight: bold;
   margin-top: 2em;
}

.boldred {
	font-weight: bold;
	color: red;
}

.notbold {
	font-weight: normal;
}

BODY {
	counter-reset: counterH1 counterFig counterTable;      /* Create a chapter counter scope */
   margin: 1ex 3%;
}

H1.count:before {
	content: counter(counterH1) " ";
	counter-increment: counterH1;  /* Add 1 to chapter */
}

H1.count:after {
	content: "";
}

H1.count {
	counter-reset: counterH2;      /* Set section to 0 */
	background-color: #ddddff;
}

H2.count:before {
	content: counter(counterH1) "." counter(counterH2) " ";
	counter-increment: counterH2;
}

H2.count {
	counter-reset: counterH3;      /* Set section to 0 */
	background-color: #dfdfff;
}

H3.count:before {
	content: counter(counterH1) "." counter(counterH2) "." counter(counterH3) " ";
	counter-increment: counterH3;
}

H3.count {
	counter-reset: counterH4;      /* Set section to 0 */
	background-color: #ededff;
}

H4.count:before {
	content: counter(counterH1) "." counter(counterH2) "." counter(counterH3) "." counter(counterH4) " ";
	counter-increment: counterH4;
}

H4.count {
	counter-reset: counterH5;      /* Set section to 0 */
	background-color: #efefff;
}

H5.count:before {
	content: counter(counterH1) "." counter(counterH2) "." counter(counterH3) "." counter(counterH4) "." counter(counterH5) " ";
	counter-increment: counterH5;
}

H5.count {
	background-color: #efefff;
}

OL.count { counter-reset: item }
LI.count { display: block }
LI.count:before { content: counters(item, ".") " "; counter-increment: item }


.figcount:before {
	content: "Figure " counter(counterFig) ": ";
	counter-increment: counterFig;  
	font-weight: bold;
}

.figure {
   display: inline;
}

div.tablecount:before {
	content: "Table " counter(counterTable) ": ";
	counter-increment: counterTable;  
	font-weight: bold;
}

.hidden { display: none; }
.unhidden { display: block; }

div.divcenter {
   width: 50%;
   margin: 0px auto;
}

div.section {
   display: block;
}

.padding0 td {
   padding: 0em !important;
}

.padding1 {
   padding: 0.2em;
}

.padding2 {
   padding: 0.5em;
}

table.generic {
   border-collapse:collapse;
}

table.generic th {
   vertical-align:text-top;
   text-align: left;
   border: 1px solid black;
   padding: 0.5em;
}

table.generic td {
   vertical-align:text-top;
   border: 1px solid black;
   padding: 0.5em;
}

table.generic_dl_0 {
   border-collapse:collapse;
   border-style: none;
}

table.generic_dl_0 th {
   vertical-align:text-top;
   text-align: left;
   border-style: none;
   padding: 0.5em;
}

table.generic_dl_0 td {
   vertical-align:text-top;
   border-style: none;
   padding: 0.5em;
}


table.simple0 td, th {
   margin-left: auto;
   margin-right: auto;
   padding: 0;
   border: 1px solid black;
   border-collapse:collapse;
}

table.simple00 {
   margin-left: auto;
   margin-right: auto;
   padding: 0;
   border: 0px none white;
   table-layout: auto;
   width: 1%;
}

table.simple00 td, th {
	text-align: center;
	vertical-align: top;
}

table.w80 {
   width: 80% !important;
}

table.w100 {
   width: 100% !important;
}


table.simple {
   margin-left: auto;
   margin-right: auto;
   padding: 0.3em;
   border-spacing: 0px;
   background-color: #ffffff;
}

table.simple td, th {
   border: 1px solid #aaa;
   padding: 0.3em;
   background-color: #fff;
}

tr.toprow2 td {
   width: 50%;
}

tr.toprow3 td {
   width: 33%;
}

tr.toprow4 td {
   width: 25%;
}

tr.toprow5 td {
   width: 20%;
}

tr.toprow6 td {
   width: 20%;
}

tr.my_tr_highlight:hover td {
  background-color: #E9E9E9; color: #000;
}

tr.my_tr_highlight:hover .bluPol {
  background-color: #6f9fdf !important;
}

tr.my_tr_highlight:hover .yelPol {
  background-color: #cfcf8f !important;
}
