# Containers

## Building the Container(s) on SCC

The SCC only supports the use of **Singularity** as a container technology. The BU TechSupport website already has a nice set of articles on building and using containers on the SCC \[[Building Containers](https://www.bu.edu/tech/support/research/software-and-programming/containers/building/)\]. If you are using singularity containers for the first time, it is highly recommended to check them out \[[Containers](https://www.bu.edu/tech/support/research/software-and-programming/containers/)\].

The SCC hosts dedicated build nodes, use these to build the *`.sif`* singularity image file from the container definition file `.def`


**SSH into one of the build nodes (*`scc-i01`* or *`scc-i02`*) :**

```bash
   [usr@scc1 ~] ssh scc-i01
   ```


**Build the container :**

> set **PROJECT_NAME** to the directory path containing the cloned **adi-wf** repository.

Copy the definition file into the user scratch space. Build the image. Move the built image back to the containers directory inside your project space, **$PROJECT_NAME**.
```bash
   [usr@scc-i01 ~] mkdir $TMPDIR/$USER
   [usr@scc-i01 ~] SING_DIR=$TMPDIR/$USER
   [usr@scc-i01 ~] cd $SING_DIR

   [usr@scc-i01 $USER] cp $PROJECT_NAME/adi-wf/containers/sing_uboot_ubuntu20.04.def ./
   [usr@scc-i01 $USER] singularity build --fakeroot sing_adiwf_ubutu20.04.v03.sif sing_uboot_ubuntu20.04.def
   [usr@scc-i01 $USER] mv -b $SING_DIR/sing_adiwf_ubutu20.04.v03.sif $PROJECT_NAME/adi-wf/containers/
   ```


**Clean-up and exit :** 
```bash
   [usr@scc-i01 $USER] cd 
   [usr@scc-i01 ~] rm -rf $SING_DIR
   [usr@scc-i01 ~] exit
   ```

## Building the Container(s) on host machine
