<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.2.0" name="ts" tilewidth="21" tileheight="21" spacing="2" margin="2" tilecount="900" columns="30">
 <properties>
  <property name="uri" value="@ts"/>
 </properties>
 <image source="../img/spritesheet.png" trans="ff00ff" width="692" height="692"/>
 <tile id="11">
  <objectgroup draworder="index">
   <object id="1" x="0" y="10.5" width="21" height="10.5">
    <properties>
     <property name="class" value="death"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="70">
  <objectgroup draworder="index">
   <object id="2" x="0" y="10.5" width="21" height="10.5">
    <properties>
     <property name="class" value="death"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="77">
  <properties>
   <property name="class" value="pickup"/>
   <property name="value" type="int" value="50"/>
  </properties>
  <objectgroup draworder="index">
   <object id="1" x="5.25" y="5.25" width="10.5" height="10.5">
    <properties>
     <property name="class" value="pickup"/>
     <property name="value" type="int" value="0"/>
    </properties>
    <ellipse/>
   </object>
  </objectgroup>
 </tile>
 <tile id="78">
  <properties>
   <property name="class" value="pickup"/>
   <property name="value" type="int" value="100"/>
  </properties>
  <objectgroup draworder="index">
   <object id="2" x="5.25" y="5.25" width="10.5" height="10.5">
    <properties>
     <property name="class" value="pickup"/>
     <property name="value" type="int" value="100"/>
    </properties>
    <ellipse/>
   </object>
  </objectgroup>
 </tile>
 <tile id="122">
  <objectgroup draworder="index">
   <object id="1" x="0" y="0" width="21" height="21">
    <properties>
     <property name="class" value="env"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="123">
  <objectgroup draworder="index">
   <object id="2" x="0" y="0" width="21" height="21">
    <properties>
     <property name="class" value="env"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="124">
  <objectgroup draworder="index">
   <object id="1" x="0" y="0" width="21" height="21">
    <properties>
     <property name="class" value="env"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="283">
  <objectgroup draworder="index">
   <object id="1" x="0" y="10.5" width="21" height="10.5">
    <properties>
     <property name="class" value="bounce"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="361">
  <objectgroup draworder="index">
   <object id="1" x="0" y="0" width="21" height="10.5">
    <properties>
     <property name="class" value="env"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="362">
  <objectgroup draworder="index">
   <object id="1" x="0" y="0" width="21" height="10.5">
    <properties>
     <property name="class" value="env"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="363">
  <objectgroup draworder="index">
   <object id="1" x="0" y="0" width="21" height="10.5">
    <properties>
     <property name="class" value="env"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="364">
  <objectgroup draworder="index">
   <object id="1" x="0" y="0" width="21" height="10.5">
    <properties>
     <property name="class" value="env"/>
    </properties>
   </object>
  </objectgroup>
 </tile>
 <tile id="748">
  <objectgroup draworder="index">
   <object id="1" x="0" y="0" width="21" height="21">
    <properties>
     <property name="class" value="spawnPoint"/>
    </properties>
    <ellipse/>
   </object>
  </objectgroup>
 </tile>
</tileset>
