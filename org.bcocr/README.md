# **Business Card OCR**

##  **Introduction**

#### **About**

The following component parses data from a business card reader in accordance
with the specifications detailed in [Software Requirement Specifications.pdf](https://github.com/Unachieved/Projects/tree/main/org.bcocr/Software%20Requirements%20Specification.pdf). 

The project uses:

* JavaSE 1.8 -- java compiler
* Java 8+ -- lambdas
* Javafx 12.0.2 -- gui
* Maven 3.6.3 -- packaging
    * checkstyle -- style enforcer

#### Platform Compliance

The component has been tested on:
* Mac OS X - mojave

## **Setup**

Requirements for the component may be attained from the following links:

**Java:**  [Get Java](https://www.java.com/en/)

**Maven:** [Get Maven](https://maven.apache.org/index.html)

## **Usage**
*Requires: JavaSE-1.8, Apache Maven 3.6.3*

All commands should be executed within the root directory of the org.bcocr
project using a bash terminal

#### **To Run:** 
```
mvn package
mvn javafx:run
```

#### **To Test:**
```
mvn test
```
#### **When running**

**step 0:** Gui is visible. Click Add Contact
![state 0](https://github.com/Unachieved/Projects/tree/main/org.bcocr/imgs/state1.png)

**step 1:** Enter Data
![state 1](https://github.com/Unachieved/Projects/tree/main/org.bcocr/imgs/state2.png)

**step 2:** Click Submit
![state 2](https://github.com/Unachieved/Projects/tree/main/org.bcocr/imgs/state3.png)

**done**
![state 3](https://github.com/Unachieved/Projects/tree/main/org.bcocr/imgs/state4.png)

*For more information regarding the component such as implementation details and 
assumptions, view the 
[Software Requirement Specifications](https://github.com/Unachieved/Asymmetrik/blob/main/Software%20Requirements%20Specification.pdf) document*