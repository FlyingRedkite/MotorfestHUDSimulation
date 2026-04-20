# RaceHUD Multiplayer
**UE5.7 · C++ · Réplication**

Prototype technique d'un HUD multijoueur en Unreal Engine 5.7, entièrement en C++. Focus sur l'architecture réseau UE5 : réplication de variables, Server/Client RPC, serveur autoritaire. Pas de physique voiture — le projet cible délibérément la couche réseau et UI, pas la mécanique de jeu.

---

## Contexte

Projet de suivi du simulateur ImGui, cette fois dans un contexte moteur réel. L'objectif : transposer les concepts de synchronisation entre threads (mutex, copie locale, autorité d'écriture) vers les mécanismes de réplication UE5, et construire un HUD par client correctement découplé des données serveur.

---

## Ce que fait le projet

Un serveur autoritaire UE5 fait tourner une partie avec 2 à 4 cubes contrôlables. Chaque client dispose d'un HUD indépendant affichant sa vitesse locale, le classement général et le ping de chaque joueur.

```
┌─────────────────────────────────────────┐
│         SERVEUR (autorité)              │
│  AGameMode::Tick() toutes les 500ms     │
│  → calcule classement par distance      │
│  → met à jour AGameState + APlayerState │
│  → réplication automatique UE5          │
└──────────────┬──────────────────────────┘
               │
       ┌───────┴───────┐
       ▼               ▼
  [Client A]       [Client B]
  HUD :            HUD :
  - Vitesse*       - Vitesse*
  - Classement     - Classement
  - Pings          - Pings

* lue depuis le Pawn local, sans passer par le réseau
```

---

## Architecture des classes

| Classe | Rôle | Existe où |
|---|---|---|
| `Motorfest_HUD_multiGameMode` | Autorité, calcule le classement | Serveur uniquement |
| `ARaceGameState` | Classement et temps — répliqués | Serveur + tous clients |
| `ARacePlayerState` | Vitesse, rang, distance — répliqués | Serveur + tous clients |
| `Motorfest_HUD_multiCharacter` | Cube contrôlable, envoie sa vitesse et distance au serveur | Serveur + tous clients |
| `Motorfest_HUD_multiPlayerController` | Crée le HUD, reçoit les Client RPC | Client uniquement |
| `URaceHUDWidget` | Lit les données répliquées, affiche le HUD | Client uniquement |
| `AFinishLine` | Box trigger, détecte l'arrivée côté serveur | Serveur + tous clients |

---

## Concepts techniques couverts

### Réplication
- Variables `UPROPERTY(Replicated)` + `DOREPLIFETIME` — synchronisation automatique serveur → clients
- Autorité serveur — seul le serveur écrit dans les variables répliquées
- `PlayerArray` dans `AGameState` — liste maintenue automatiquement par UE5

### RPC (Remote Procedure Calls)
- **Server RPC** (`ServerUpdateSpeed`, `ServerUpdateDistance`) — le client demande au serveur de mettre à jour ses données
- **Client RPC** (`ClientShowFinished`) — le serveur envoie un événement à un client précis (affichage du widget "Finish !")

### HUD et performance
- `NativeConstruct()` pour cacher les refs coûteuses (cast fait une seule fois)
- Vitesse locale lue depuis `GetVelocity()` sur le Pawn — zéro latence réseau, pas de round-trip serveur
- Classement reconstruit toutes les 500ms, pas à chaque frame
- Delta compression sur les RPC de vitesse (seuil 5 cm/s)

### Patterns réseau
- **Client-side prediction** : la vitesse affichée est locale, pas répliquée — élimine le délai perceptible
- **Serveur autoritaire** : le classement est calculé et distribué par le serveur, pas par chaque client
- **Network Emulation** UE5 utilisé pour tester avec latence et packet loss simulés

---

## Parallèles avec le projet ImGui

| Projet ImGui | Projet UE5 | Concept commun                                           |
|---|---|----------------------------------------------------------|
| `networkThread()` toutes les 50ms | `AGameMode::Tick()` toutes les 500ms | Mise à jour autoritaire périodique                       |
| `std::mutex` + copie locale | Réplication UE5 | Synchronisation de l'état du jeu sans corruption des données |
| Écriture réservée au thread réseau | Écriture réservée au serveur | Autorité d'écriture unique                               |
| `isLocal` dans `PlayerData` | `IsLocalController()` | Distinguer le joueur local                               |
| Packet loss simulé (5%) | Network Emulation UE5 | Tester l'UX du HUD                                       |

---

## Stack

- **Moteur** : Unreal Engine 5.7
- **Techno** : C++ (logique réseau et HUD), UMG (layout visuel uniquement)
- **IDE** : JetBrains Rider
- **Template de base** : Third Person C++

## Lancer le projet

1. Ouvrir `Motorfest_HUD_multi.uproject` dans UE5.7
2. Compiler depuis Rider ou VS
3. Lancer le projet avec l'option ""
