
add_custom_target(data_folder
   COMMAND ${CMAKE_COMMAND} -E make_directory "data/"
   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_target(uni
   COMMAND $<TARGET_FILE:admin> initdb uni
   COMMAND $<TARGET_FILE:admin> createtable uni professoren persnr key int name string rang string raum int
   COMMAND $<TARGET_FILE:admin> createtable uni studenten matrnr key int name string semester int
   COMMAND $<TARGET_FILE:admin> createtable uni vorlesungen vorlnr key int titel string sws int gelesenvon int
   COMMAND $<TARGET_FILE:admin> createtable uni voraussetzen vorgaenger int nachfolger int
   COMMAND $<TARGET_FILE:admin> createtable uni hoeren matrnr int vorlnr int
   COMMAND $<TARGET_FILE:admin> createtable uni assistenten persnr key int name string fachgebiet string boss int
   COMMAND $<TARGET_FILE:admin> createtable uni pruefen matrnr int vorlnr int persnr int note int
   COMMAND $<TARGET_FILE:admin> insertvalues uni professoren 2125 Sokrates C4 226
   COMMAND $<TARGET_FILE:admin> insertvalues uni professoren 2126 Russel C4 232
   COMMAND $<TARGET_FILE:admin> insertvalues uni professoren 2127 Kopernikus C3 310
   COMMAND $<TARGET_FILE:admin> insertvalues uni professoren 2133 Popper C3 52
   COMMAND $<TARGET_FILE:admin> insertvalues uni professoren 2134 Augustinus C3 309
   COMMAND $<TARGET_FILE:admin> insertvalues uni professoren 2136 Curie C4 36
   COMMAND $<TARGET_FILE:admin> insertvalues uni professoren 2137 Kant C4 7
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 24002 Xenokrates 18
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 25403 Jonas 12
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 26120 Fichte 10
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 26830 Aristoxenos 8
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 27550 Schopenhauer 6
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 28106 Carnap 3
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 29120 Theophrastos 2
   COMMAND $<TARGET_FILE:admin> insertvalues uni studenten 29555 Feuerbach 2
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5001 Grundzüge 4 2137
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5041 Ethik 4 2125
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5043 Erkenntnistheorie 3 2126
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5049 Mäeutik 2 2125
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 4052 Logik 4 2125
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5052 Wissenschaftstheorie 3 2126
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5216 Bioethik 2 2126
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5259 "Der Wiener Kreis" 2 2133
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 5022 "Glaube und Wissen" 2 2134
   COMMAND $<TARGET_FILE:admin> insertvalues uni vorlesungen 4630 "Die 3 Kritiken" 4 2137
   COMMAND $<TARGET_FILE:admin> insertvalues uni voraussetzen 5001 5041
   COMMAND $<TARGET_FILE:admin> insertvalues uni voraussetzen 5001 5043
   COMMAND $<TARGET_FILE:admin> insertvalues uni voraussetzen 5001 5049
   COMMAND $<TARGET_FILE:admin> insertvalues uni voraussetzen 5041 5216
   COMMAND $<TARGET_FILE:admin> insertvalues uni voraussetzen 5043 5052
   COMMAND $<TARGET_FILE:admin> insertvalues uni voraussetzen 5041 5052
   COMMAND $<TARGET_FILE:admin> insertvalues uni voraussetzen 5052 5259
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 26120 5001
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 27550 5001
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 27550 4052
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 28106 5041
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 28106 5052
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 28106 5216
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 28106 5259
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 29120 5001
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 29120 5041
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 29120 5049
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 29555 5022
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 25403 5022
   COMMAND $<TARGET_FILE:admin> insertvalues uni hoeren 29555 5001
   COMMAND $<TARGET_FILE:admin> insertvalues uni assistenten 3002 Platon Ideenlehre 2125
   COMMAND $<TARGET_FILE:admin> insertvalues uni assistenten 3003 Aristoteles Syllogistik 2125
   COMMAND $<TARGET_FILE:admin> insertvalues uni assistenten 3004 Wittgenstein Sprachtheorie 2126
   COMMAND $<TARGET_FILE:admin> insertvalues uni assistenten 3005 Rhetikus Planetenbewegung 2127
   COMMAND $<TARGET_FILE:admin> insertvalues uni assistenten 3006 Newton "Keplersche Gesetze" 2127
   COMMAND $<TARGET_FILE:admin> insertvalues uni assistenten 3007 Spinoza "Gott und Natur" 2134
   COMMAND $<TARGET_FILE:admin> insertvalues uni pruefen 28106 5001 2126 1
   COMMAND $<TARGET_FILE:admin> insertvalues uni pruefen 25403 5041 2125 2
   COMMAND $<TARGET_FILE:admin> insertvalues uni pruefen 27550 4630 2137 2
   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data"
)

add_dependencies(uni admin data_folder)