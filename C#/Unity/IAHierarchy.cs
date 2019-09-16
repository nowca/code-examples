using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using VRTK;
using VRTK.Highlighters;

namespace VENTUS {
	namespace GUI {
		public partial class IAHierarchy : MonoBehaviour {
			public static IAHierarchy instance;

			public GameObject selectedObject;
			public GameObject nodePrefab;

			public RuntimeAnimatorController normalButton;
			public RuntimeAnimatorController selectedButton;

			public static int treeDepth = 0;

			private static HierarchyNode rootNode;

			private static HierarchyNode GetRootNode(GameObject rootObject = null) {
				if (rootObject != null) {
					rootObject = new GameObject();
				} else {
					rootObject = GameObject.Find("[Models]");
				}

				HierarchyNode rootNode = new HierarchyNode(rootObject);

				return rootNode;
			}
		
			private static void BuildHierarchy(HierarchyNode parentNode, int i = 0) {
				GameObject gameObject = parentNode.gameObject;
				i++;

				foreach (Transform child in gameObject.transform) {
					if (!child.name.Contains("HighlightModel")) {
						HierarchyNode childNode = new HierarchyNode(child.gameObject, i, parentNode);
						parentNode.AddNode(childNode);
						IAHierarchy.BuildHierarchy(childNode, i);
					}
				}
			}

			private static void DestroyHierarchy(HierarchyNode parentNode) {
				Object.Destroy(parentNode.guiElement);

				foreach (HierarchyNode childNode in parentNode.nodeList) {
					IAHierarchy.DestroyHierarchy(childNode);
				}
			}
				
			private static HierarchyNode FindHierarchyNode(GameObject gameObject, HierarchyNode parentNode) {
				HierarchyNode foundNode = null;

				if (parentNode.gameObject == gameObject) {
					foundNode = parentNode;
				}

				foreach (HierarchyNode childNode in parentNode.nodeList) {
					HierarchyNode returnNode = FindHierarchyNode(gameObject, childNode);
					if (returnNode != null) { foundNode = returnNode; }
				}

				return foundNode;
			}

			private static void UnhighlightAll(HierarchyNode parentNode, bool includeActiveIAObject = false) {
				foreach (HierarchyNode childNode in parentNode.nodeList) {
					if (includeActiveIAObject || childNode.gameObject != Interaction.IAController.activeIAObject) {
						Interaction.IAController.Unhighlight(childNode.gameObject);
					}
					UnhighlightAll(childNode, includeActiveIAObject);
				}
			}
				
			public static void Init(GameObject initObject = null) {
				if (initObject != null) {
					IAHierarchy.instance.selectedObject = initObject;
				}
				IAHierarchy.Objects.hierarchyTransform = GameObject.Find("CVS_IAHierarchy").transform;
				IAHierarchy.Objects.viewportContentTransform = IAHierarchy.Objects.hierarchyTransform.Find("Scroll View/Viewport/Content");
				IAHierarchy.rootNode = IAHierarchy.GetRootNode();
				IAHierarchy.BuildHierarchy(IAHierarchy.rootNode);
				IAHierarchy.Rendering.RenderGUITree(IAHierarchy.rootNode);
				GameObject.Find("CVS_IAHierarchy").SetActive(false);
			}

			void Start () {
				IAHierarchy.instance = this;
				IAHierarchy.Init();
			}
		}
	}
}
