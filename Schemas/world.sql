CREATE DATABASE  IF NOT EXISTS `world` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;
USE `world`;
-- MySQL dump 10.13  Distrib 8.0.36, for Win64 (x86_64)
--
-- Host: localhost    Database: world
-- ------------------------------------------------------
-- Server version	8.0.37

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `collectable`
--

DROP TABLE IF EXISTS `collectable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `collectable` (
  `id` bigint unsigned NOT NULL,
  `seconds` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `collectable`
--

LOCK TABLES `collectable` WRITE;
/*!40000 ALTER TABLE `collectable` DISABLE KEYS */;
INSERT INTO `collectable` VALUES (0,1);
/*!40000 ALTER TABLE `collectable` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `collectable_spawner`
--

DROP TABLE IF EXISTS `collectable_spawner`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `collectable_spawner` (
  `map_id` bigint unsigned NOT NULL,
  `collectable_id` bigint unsigned NOT NULL,
  `count` bigint unsigned NOT NULL,
  `milliseconds` int unsigned NOT NULL,
  `argument_1` float NOT NULL,
  `argument_2` float NOT NULL,
  `argument_3` float DEFAULT NULL,
  PRIMARY KEY (`map_id`,`collectable_id`),
  KEY `collectable_spawner_collectable_idx` (`collectable_id`),
  CONSTRAINT `collectable_spawner_collectable` FOREIGN KEY (`collectable_id`) REFERENCES `collectable` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `collectable_spawner_map` FOREIGN KEY (`map_id`) REFERENCES `map` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `collectable_spawner`
--

LOCK TABLES `collectable_spawner` WRITE;
/*!40000 ALTER TABLE `collectable_spawner` DISABLE KEYS */;
INSERT INTO `collectable_spawner` VALUES (0,0,500,35000,210,135,NULL),(1,0,500,35000,210,135,NULL),(2,0,500,35000,210,135,NULL),(3,0,500,35000,210,135,NULL),(4,0,500,35000,210,135,NULL),(5,0,500,35000,210,135,NULL),(6,0,500,35000,210,135,NULL),(7,0,500,35000,210,135,NULL),(8,0,500,35000,210,135,NULL),(9,0,500,35000,210,135,NULL),(10,0,500,35000,210,135,NULL),(11,0,500,35000,210,135,NULL),(12,0,500,35000,210,135,NULL),(13,0,500,35000,210,135,NULL),(14,0,500,35000,210,135,NULL),(15,0,500,35000,210,135,NULL),(16,0,500,35000,210,135,NULL),(17,0,500,35000,210,135,NULL),(18,0,500,35000,210,135,NULL),(19,0,500,35000,210,135,NULL),(20,0,500,35000,210,135,NULL),(21,0,500,35000,210,135,NULL),(22,0,500,35000,210,135,NULL),(23,0,500,35000,210,135,NULL),(24,0,500,35000,210,135,NULL),(25,0,500,35000,210,135,NULL),(26,0,500,35000,210,135,NULL);
/*!40000 ALTER TABLE `collectable_spawner` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `map`
--

DROP TABLE IF EXISTS `map`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `map` (
  `id` bigint unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `map`
--

LOCK TABLES `map` WRITE;
/*!40000 ALTER TABLE `map` DISABLE KEYS */;
INSERT INTO `map` VALUES (0),(1),(2),(3),(4),(5),(6),(7),(8),(9),(10),(11),(12),(13),(14),(15),(16),(17),(18),(19),(20),(21),(22),(23),(24),(25),(26),(27),(28);
/*!40000 ALTER TABLE `map` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `map_area`
--

DROP TABLE IF EXISTS `map_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `map_area` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `map_id` bigint unsigned NOT NULL,
  `position_x` float NOT NULL,
  `position_y` float NOT NULL,
  `radius` float NOT NULL,
  `script` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  KEY `map_area_idx` (`map_id`),
  CONSTRAINT `map_area` FOREIGN KEY (`map_id`) REFERENCES `map` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `map_area`
--

LOCK TABLES `map_area` WRITE;
/*!40000 ALTER TABLE `map_area` DISABLE KEYS */;
/*!40000 ALTER TABLE `map_area` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc`
--

DROP TABLE IF EXISTS `npc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc` (
  `id` bigint unsigned NOT NULL,
  `max_hp` int unsigned NOT NULL,
  `max_shield` int unsigned NOT NULL,
  `hp` int unsigned NOT NULL,
  `shield` int unsigned NOT NULL,
  `speed` float NOT NULL,
  `min_damage` int unsigned NOT NULL,
  `max_damage` int unsigned NOT NULL,
  `attack_radius` float NOT NULL,
  `ai` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `faction` tinyint unsigned NOT NULL,
  `attack_time` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `npc`
--

LOCK TABLES `npc` WRITE;
/*!40000 ALTER TABLE `npc` DISABLE KEYS */;
INSERT INTO `npc` VALUES (0,800,400,800,400,280,15,20,4.1,'Defensive',3,1),(1,2000,2000,2000,2000,320,60,80,4.1,'FullAggresive',3,1),(2,6000,3000,6000,3000,320,150,200,4.1,'FullAggresive',3,1),(3,20000,10000,20000,10000,125,300,400,4.1,'FullAggresive',3,1),(4,100000,100000,100000,100000,150,900,1200,4.1,'FullAggresive',3,1),(5,200000,200000,200000,200000,100,2250,3000,4.1,'FullAggresive',3,1),(6,40000,40000,40000,40000,320,750,1000,4.1,'FullAggresive',3,1),(7,300000,200000,300000,200000,230,3000,4000,4.1,'Defensive',3,1),(8,50000,40000,50000,40000,320,900,1200,4.1,'FullAggresive',3,1),(9,400000,300000,400000,300000,250,3750,5000,4.1,'Defensive',3,1),(10,40000,30000,40000,30000,280,750,1000,4.1,'Defensive',3,1);
/*!40000 ALTER TABLE `npc` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_spawner`
--

DROP TABLE IF EXISTS `npc_spawner`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_spawner` (
  `map_id` bigint unsigned NOT NULL,
  `npc_id` bigint unsigned NOT NULL,
  `count` bigint unsigned NOT NULL,
  `milliseconds` int unsigned NOT NULL,
  PRIMARY KEY (`map_id`,`npc_id`),
  KEY `npc_spawner_npc_idx` (`npc_id`),
  CONSTRAINT `npc_spawner_map` FOREIGN KEY (`map_id`) REFERENCES `map` (`id`) ON DELETE CASCADE,
  CONSTRAINT `npc_spawner_npc` FOREIGN KEY (`npc_id`) REFERENCES `npc` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `npc_spawner`
--

LOCK TABLES `npc_spawner` WRITE;
/*!40000 ALTER TABLE `npc_spawner` DISABLE KEYS */;
INSERT INTO `npc_spawner` VALUES (0,0,100,25000),(1,0,100,25000),(1,1,100,25000),(2,1,72,25000),(2,2,72,25000),(2,3,40,25000),(2,4,16,60000),(3,1,72,25000),(3,2,72,25000),(3,3,40,25000),(3,5,16,60000),(4,1,100,25000),(4,6,75,25000),(4,7,25,25000),(6,8,180,25000),(6,9,20,40000),(7,10,100,25000),(8,0,100,25000),(9,0,100,25000),(9,1,100,25000),(10,1,72,25000),(10,2,72,25000),(10,3,40,25000),(10,4,16,60000),(11,1,72,25000),(11,2,72,25000),(11,3,40,25000),(11,5,16,60000),(12,1,100,25000),(12,6,75,25000),(12,7,25,25000),(14,8,180,25000),(14,9,20,40000),(15,10,100,25000),(16,0,100,25000),(17,0,100,25000),(17,1,100,25000),(18,1,72,25000),(18,2,72,25000),(18,3,40,25000),(18,4,16,60000),(19,1,72,25000),(19,2,72,25000),(19,3,40,25000),(19,5,16,60000),(20,1,100,25000),(20,6,75,25000),(20,7,25,25000),(22,8,180,25000),(22,9,20,40000),(23,10,100,25000);
/*!40000 ALTER TABLE `npc_spawner` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `portal`
--

DROP TABLE IF EXISTS `portal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `portal` (
  `id` bigint unsigned NOT NULL,
  `map_id` bigint unsigned NOT NULL,
  `position_x` float NOT NULL,
  `position_y` float NOT NULL,
  `destination` bigint unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `portal_map_idx` (`map_id`),
  KEY `portal_destination_portal` (`destination`),
  CONSTRAINT `portal_destination_portal` FOREIGN KEY (`destination`) REFERENCES `portal` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `portal_map` FOREIGN KEY (`map_id`) REFERENCES `map` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `portal`
--

LOCK TABLES `portal` WRITE;
/*!40000 ALTER TABLE `portal` DISABLE KEYS */;
INSERT INTO `portal` VALUES (0,0,190,-115,2),(1,0,40,-40,85),(2,1,20,-20,0),(3,1,190,-20,5),(4,1,190,-115,8),(5,2,20,-115,3),(6,2,190,-115,9),(7,2,190,-20,28),(8,3,20,-20,4),(9,3,190,-20,6),(10,3,190,-115,50),(11,3,190,-67.5,66),(12,4,20,-20,16),(13,4,20,-115,19),(14,4,190,-67.5,78),(15,4,105,-115,84),(16,5,190,-115,12),(17,5,20,-115,20),(18,6,20,-20,21),(19,6,190,-20,13),(20,7,190,-20,17),(21,7,190,-115,18),(22,8,20,-115,23),(23,9,190,-20,22),(24,9,190,-115,29),(25,9,20,-115,26),(26,10,190,-20,25),(27,10,190,-115,30),(28,10,20,-115,7),(29,11,20,-20,24),(30,11,190,-20,27),(31,11,20,-115,47),(32,11,105,-115,71),(33,12,20,-20,38),(34,12,190,-20,40),(35,12,190,-115,82),(36,12,20,-115,76),(37,13,190,-20,42),(38,13,20,-115,33),(39,14,190,-20,41),(40,14,20,-67.5,34),(41,15,190,-115,39),(42,15,20,-115,37),(43,16,20,-20,46),(44,17,20,-20,52),(45,17,190,-20,48),(46,17,190,-115,43),(47,18,20,-20,31),(48,18,190,-115,45),(49,18,20,-115,51),(50,19,20,-20,10),(51,19,190,-20,49),(52,19,190,-115,44),(53,19,105,-20,75),(54,20,190,-115,60),(55,20,20,-115,58),(56,20,105,-20,83),(57,20,20,-20,77),(58,21,20,-20,55),(59,21,190,-115,63),(60,22,20,-20,54),(61,22,190,-115,62),(62,23,20,-20,61),(63,23,20,-115,59),(64,24,190,-20,69),(65,24,190,-115,73),(66,24,20,-67.5,11),(67,24,105,-67.5,79),(68,25,190,-115,72),(69,25,20,-115,64),(70,25,105,-67.5,80),(71,25,105,-20,32),(72,26,20,-20,68),(73,26,20,-115,65),(74,26,105,-67.5,81),(75,26,190,-67.5,53),(76,27,190,-20,36),(77,27,190,-115,57),(78,27,20,-67.5,14),(79,27,52.5,-67.5,67),(80,27,157.5,-33.75,70),(81,27,157.5,-101.25,74),(82,28,190,-20,35),(83,28,190,-115,56),(84,28,20,-67.5,15),(85,28,105,-67.5,1);
/*!40000 ALTER TABLE `portal` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-11-21 12:05:55
