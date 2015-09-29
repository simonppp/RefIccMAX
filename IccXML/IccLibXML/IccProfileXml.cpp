/** @file
    File:       IccProfileXml.cpp

    Contains:   Implementation Icc Profile XML format conversions

    Version:    V1

    Copyright:  � see ICC Software License
*/

/*
 * The ICC Software License, Version 0.2
 *
 *
 * Copyright (c) 2003-2012 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. In the absence of prior written permission, the names "ICC" and "The
 *    International Color Consortium" must not be used to imply that the
 *    ICC organization endorses or promotes products derived from this
 *    software.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */
#include "stdio.h"
#include "IccProfileXml.h"
#include "IccTagXml.h"
#include "IccUtilXml.h"
#include "IccArrayBasic.h"
#include <set>
#include <cstring> /* C strings strcpy, memcpy ... */

bool CIccProfileXml::ToXml(std::string &xml)
{ 
  CIccInfo info;
  char line[256];
  char buf[256];
  char fix[256];
  int n;
  bool nonzero;
  
  xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  xml += "<IccProfile>\n";
  xml += "  <Header>\n";
  sprintf(line, "    <PreferredCMMType>%s</PreferredCMMType>\n", icFixXml(fix, icGetColorSigStr(buf, m_Header.cmmId)));
  xml += line;
  sprintf(line, "    <ProfileVersion>%s</ProfileVersion>\n", info.GetVersionName(m_Header.version));
  xml += line;
  sprintf(line, "    <ProfileDeviceClass>%s</ProfileDeviceClass>\n", icFixXml(fix, icGetSigStr(buf, m_Header.deviceClass)));
  xml += line;
  sprintf(line, "    <DataColourSpace>%s</DataColourSpace>\n", icFixXml(fix, icGetSigStr(buf, m_Header.colorSpace)));
  xml += line;
  sprintf(line, "    <PCS>%s</PCS>\n",  icFixXml(fix, icGetColorSigStr(buf, m_Header.pcs)));
  xml += line;

  sprintf(line, "    <CreationDateTime>%d-%02d-%02dT%02d:%02d:%02d</CreationDateTime>\n", 
										m_Header.date.year,
										m_Header.date.month,
										m_Header.date.day,
										m_Header.date.hours,
										m_Header.date.minutes,
										m_Header.date.seconds);
  xml += line;

 // if (m_Header.magic != 0){
 //	  sprintf(line, "    <Signature>%s</Signature>\n", icFixXml(fix, icGetSigStr(buf, m_Header.magic)));
 //	  xml += line;
 // }

  if (m_Header.platform != icSigUnkownPlatform){
	sprintf(line, "    <PrimaryPlatform>%s</PrimaryPlatform>\n", icFixXml(fix, icGetSigStr(buf, m_Header.platform)));
	xml += line;
  }
 
  xml+= "    ";
  xml+= icGetHeaderFlagsName(m_Header.flags);

  if (m_Header.manufacturer != 0){
	  sprintf(line, "    <DeviceManufacturer>%s</DeviceManufacturer>\n", icFixXml(fix, icGetSigStr(buf, m_Header.manufacturer)));
	  xml += line;
  }
  
  if (m_Header.model != 0){
	sprintf(line, "    <DeviceModel>%s</DeviceModel>\n", icFixXml(fix, icGetSigStr(buf, m_Header.model)));
	xml += line;
  }

  xml+= "    ";
  xml += icGetDeviceAttrName(m_Header.attributes);

  sprintf(line, "    <RenderingIntent>%s</RenderingIntent>\n", info.GetRenderingIntentName((icRenderingIntent)m_Header.renderingIntent));
  xml += line;
  sprintf(line, "    <PCSIlluminant>\n      <XYZNumber X=\"%.8f\" Y=\"%.8f\" Z=\"%.8f\"/>\n    </PCSIlluminant>\n", (float)icFtoD(m_Header.illuminant.X),
                                                             (float)icFtoD(m_Header.illuminant.Y),
                                                             (float)icFtoD(m_Header.illuminant.Z));

  xml += line;
  
  sprintf(line, "    <ProfileCreator>%s</ProfileCreator>\n", icFixXml(fix, icGetSigStr(buf, m_Header.creator)));
  xml += line;

  if (m_Header.profileID.ID32[0] || m_Header.profileID.ID32[1] || 
      m_Header.profileID.ID32[2] || m_Header.profileID.ID32[3]) {
    for (n=0; n<16; n++) {
      sprintf(buf + n*2, "%02X", m_Header.profileID.ID8[n]);
    }
    buf[n*2]='\0';
    xml += "    <ProfileID>";
    xml += buf;
    xml += "</ProfileID>\n";
  }
  nonzero = false;

  if (m_Header.spectralPCS) {
    sprintf(line, "    <SpectralPCS>%s</SpectralPCS>\n",  icFixXml(fix, icGetColorSigStr(buf, m_Header.spectralPCS)));
    xml += line;

    if (m_Header.spectralRange.steps) {
      xml += "    <SpectralRange>\n";
      sprintf(line, "     <Wavelengths start=\"%.8f\" end=\"%.8f\" steps=\"%d\"/>\n)", 
              icF16toF(m_Header.spectralRange.start), icF16toF(m_Header.spectralRange.end), m_Header.spectralRange.steps);
      xml += "    </SpectralRange>\n";
    }
    if (m_Header.biSpectralRange.steps) {
      xml += "    <BiSpectralRange>\n";
      sprintf(line, "     <Wavelengths start=\"%.8f\" end=\"%.8f\" steps=\"%d\"/>\n)", 
              icF16toF(m_Header.biSpectralRange.start), icF16toF(m_Header.biSpectralRange.end), m_Header.biSpectralRange.steps);
      xml += "    </BiSpectralRange>\n";
    }
  }

  if (m_Header.mcs) {
    sprintf(line, "    <MCS>%s</MCS>\n",  icFixXml(fix, icGetColorSigStr(buf, m_Header.mcs)));
  }

  if (m_Header.deviceSubClass) {
    sprintf(line, "    <ProfileDeviceSubClass>%s</ProfileDeviceSubClass>\n",  icFixXml(fix, icGetSigStr(buf, m_Header.deviceSubClass)));
  }

  for (n=0; n<sizeof(m_Header.reserved); n++) {
    if (m_Header.reserved[n])
      nonzero = true;
    sprintf(buf + n*2, "%02X", m_Header.reserved[n]);
  }
  buf[n*2]='\0';
  if (nonzero) {
    xml += "    <Reserved>";
    xml += buf;
	xml += "</Reserved>\n";
  }
  xml += "  </Header>\n";
  
  xml += "  <Tags>\n";
  TagEntryList::iterator i, j;
  std::set<icTagSignature> sigSet;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    if (sigSet.find(i->TagInfo.sig)==sigSet.end()) {
      CIccTag *pTag = FindTag(i->TagInfo.sig);

      if (pTag) {
        CIccTagXml *pTagXml = (CIccTagXml*)(pTag->GetExtension());
        if (pTagXml) {
          const icChar* tagSig = icGetTagSigTypeName(pTag->GetType());
		  
	        // PrivateType - a type that does not belong to the list in the icc specs - custom for vendor.
	        if ( "PrivateType" == tagSig )
		       sprintf(line, "    <PrivateType type=\"%s\">\n",  icFixXml(fix, icGetSigStr(buf, pTag->GetType())));		
	        else
		      sprintf(line, "    <%s>\n",  tagSig); //parent node is the tag type
    		  
	        xml += line; 				
              j=i;          
	        // print out the tag signature (there is at least one)
          sprintf(line, "      <TagSignature>%s</TagSignature>\n", icFixXml(fix, icGetSigStr(buf, i->TagInfo.sig)));
          xml += line;

          sigSet.insert(i->TagInfo.sig);

		       // print out the rest of the tag signatures
          for (j++; j!=m_Tags->end(); j++) {
            if (j->pTag == i->pTag || j->TagInfo.offset == i->TagInfo.offset) {
              sprintf(line, "      <TagSignature>%s</TagSignature>\n", icFixXml(fix, icGetSigStr(buf, j->TagInfo.sig)));
              xml += line;
              sigSet.insert(j->TagInfo.sig);        
            }
          }

    		  //convert the rest of the tag to xml
          if (!pTagXml->ToXml(xml, "      ")) {
            printf("Unable to output tag with type %s\n", icGetSigStr(buf, i->TagInfo.sig));
            return false;
          }
          sprintf(line, "    </%s>\n",  tagSig);
		      xml += line; 	
        }
        else {
          printf("Non XML tag in list with type %s!\n", icGetSigStr(buf, i->TagInfo.sig));
          return false;
        }
      }
      else {
        printf("Unable to find tag with type %s!\n", icGetSigStr(buf, i->TagInfo.sig));
        return false;
      }
    }
  }
  xml += "  </Tags>\n";
  xml += "</IccProfile>\n";

  return true;
}


/**
*****************************************************************************
* Name: CIccProfileXml::ParseBasic
* 
* Purpose: Parse ICC header.
* 
* Args: 
*  pNode - pointer to xmlNode object to read data with
* 
* Return: 
*  true - valid ICC header, false - failure
******************************************************************************
*/
bool CIccProfileXml::ParseBasic(xmlNode *pNode, std::string &parseStr)
{  	  
  std::string temp;
  memset(&m_Header, 0, sizeof(m_Header));  

  for (pNode=pNode->children; pNode; pNode=pNode->next) {
	  if (pNode->type==XML_ELEMENT_NODE) {
		if (!icXmlStrCmp((const char*)pNode->name, "ProfileVersion")) {
		  icFloatNumber v = (icFloatNumber)atof((const char*)pNode->children->content);

		  m_Header.version = 0;
		  icFloatNumber divisor = 10.0;
		  int i;
		  icUInt32Number units;

		  for (i=0; i<8; i++) {
			units = (icUInt32Number)(v / divisor + 0.001);
			v-= divisor * units;
			divisor /= 10.0f;

			m_Header.version += units << ((7-i)*4);
		  }
		}
		else if (!icXmlStrCmp(pNode->name, "PreferredCMMType")) {			
			m_Header.cmmId = icXmlGetChildSigVal(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "ProfileDeviceClass")) {
			m_Header.deviceClass = (icProfileClassSignature)icXmlGetChildSigVal(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "DataColourSpace")) {
			m_Header.colorSpace = (icColorSpaceSignature)icXmlGetChildSigVal(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "PCS")) {
		  m_Header.pcs = (icColorSpaceSignature)icXmlGetChildSigVal(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "CreationDateTime")) {
      if (pNode && pNode->children && pNode->children->content) {
        const char *datetime = (const char*)pNode->children->content;
			  m_Header.date = icGetDateTimeValue(datetime);
      }
      else
        memset(&m_Header.date, 0, sizeof(m_Header.date));
		}
		else if (!icXmlStrCmp(pNode->name, "PrimaryPlatform")) {
			m_Header.platform = (icPlatformSignature)icXmlGetChildSigVal(pNode);
		}		
		else if (!icXmlStrCmp(pNode->name, "ProfileFlags")) {
			m_Header.flags = 0;			
			xmlAttr *attr = icXmlFindAttr(pNode, "EmbeddedInFile");
      if (attr && !strcmp(icXmlAttrValue(attr), "true")) {
				m_Header.flags |= icEmbeddedProfileTrue; 
			}

			attr = icXmlFindAttr(pNode, "UseWithEmbeddedDataOnly");
      if (attr && !strcmp(icXmlAttrValue(attr), "true")) {
					m_Header.flags |= icUseWithEmbeddedDataOnly;
			}

      attr = icXmlFindAttr(pNode, "VendorFlags");
      if (attr) {
        icUInt32Number vendor;
        sscanf(icXmlAttrValue(attr), "%x", &vendor);
        m_Header.flags |= vendor;
      }
		}
		else if (!icXmlStrCmp(pNode->name, "DeviceManufacturer")) {
		  m_Header.manufacturer = icXmlGetChildSigVal(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "DeviceModel")) {
		  m_Header.model = icXmlGetChildSigVal(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "DeviceAttributes")) {
			m_Header.attributes = icGetDeviceAttrValue(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "RenderingIntent")) {
			if (!strcmp((const char*)pNode->children->content, "Perceptual"))
				m_Header.renderingIntent = icPerceptual;
			else if (!strcmp((const char*)pNode->children->content, "Relative Colorimetric"))
				m_Header.renderingIntent = icRelativeColorimetric;
			else if (!strcmp((const char*)pNode->children->content, "Saturation"))
				m_Header.renderingIntent = icSaturation;
			else if (!strcmp((const char*)pNode->children->content, "Absolute Colorimetric"))
				m_Header.renderingIntent = icAbsoluteColorimetric;

		}
		else if (!icXmlStrCmp(pNode->name, "PCSIlluminant")) { 
			xmlNode *xyzNode = icXmlFindNode(pNode->children, "XYZNumber");

			xmlAttr *x = icXmlFindAttr(xyzNode, "X");
			xmlAttr *y = icXmlFindAttr(xyzNode, "Y");
			xmlAttr *z = icXmlFindAttr(xyzNode, "Z");

			if (x && y && z) {
			   m_Header.illuminant.X = icDtoF((icFloatNumber)atof(icXmlAttrValue(x)));
			   m_Header.illuminant.Y = icDtoF((icFloatNumber)atof(icXmlAttrValue(y)));
			   m_Header.illuminant.Z = icDtoF((icFloatNumber)atof(icXmlAttrValue(z)));
			}
		}
		else if (!icXmlStrCmp(pNode->name, "ProfileCreator")) {
		  m_Header.creator = icXmlGetChildSigVal(pNode);
		}
		else if (!icXmlStrCmp(pNode->name, "ProfileID")) {
      if (pNode->children && pNode->children->content) 
		    icXmlGetHexData(&m_Header.profileID.ID8, (const char*)pNode->children->content, sizeof(m_Header.profileID.ID8));
      else
        memset(&m_Header.profileID.ID8, 0, sizeof(m_Header.profileID.ID8));
		}
    else if (!icXmlStrCmp(pNode->name, "SpectralPCS")) {
      m_Header.spectralPCS = (icSpectralColorSignature)icXmlGetChildSigVal(pNode);
    }
    else if (!icXmlStrCmp(pNode->name, "SpectralRange")) {
      xmlNode *xyzNode = icXmlFindNode(pNode->children, "Wavelengths");

      xmlAttr *start = icXmlFindAttr(xyzNode, "start");
      xmlAttr *end = icXmlFindAttr(xyzNode, "end");
      xmlAttr *steps = icXmlFindAttr(xyzNode, "steps");

      if (start && end && steps) {
        m_Header.spectralRange.start = icFtoF16((icFloatNumber)atof(icXmlAttrValue(start)));
        m_Header.spectralRange.end = icFtoF16((icFloatNumber)atof(icXmlAttrValue(end)));
        m_Header.spectralRange.steps = (icUInt16Number)atoi(icXmlAttrValue(steps));
      }
    }
    else if (!icXmlStrCmp(pNode->name, "BiSpectralRange")) {
      xmlNode *xyzNode = icXmlFindNode(pNode->children, "Wavelengths");

      xmlAttr *start = icXmlFindAttr(xyzNode, "start");
      xmlAttr *end = icXmlFindAttr(xyzNode, "end");
      xmlAttr *steps = icXmlFindAttr(xyzNode, "steps");

      if (start && end && steps) {
        m_Header.biSpectralRange.start = icFtoF16((icFloatNumber)atof(icXmlAttrValue(start)));
        m_Header.biSpectralRange.end = icFtoF16((icFloatNumber)atof(icXmlAttrValue(end)));
        m_Header.biSpectralRange.steps = (icUInt16Number)atoi(icXmlAttrValue(steps));
      }
    }
    else if (!icXmlStrCmp(pNode->name, "MCS")) {
      m_Header.mcs = (icMaterialColorSignature)icXmlGetChildSigVal(pNode);
    }
    else if (!icXmlStrCmp(pNode->name, "ProfileDeviceSubClass")) {
      m_Header.deviceSubClass = (icProfileClassSignature)icXmlGetChildSigVal(pNode);
    }
		else if (!icXmlStrCmp(pNode->name, "Reserved")) {
      if (pNode->children && pNode->content)
		    icXmlGetHexData(&m_Header.reserved, (const char*)pNode->children->content, sizeof(m_Header.reserved));
      else
        memset(&m_Header.reserved, 0, sizeof(m_Header.reserved));
		}
		else {
		  parseStr += "Unknown Profile Header attribute: ";
		  parseStr += (const char*)pNode->name;
		  parseStr += "=\"";
      if (pNode->children && pNode->children->content)
		    parseStr += (const char*)pNode->children->content;
		  parseStr += "\"\n";
		}
	  }
  }

  m_Header.magic = icMagicNumber;  

  return true;
}


/**
******************************************************************************
* Name: CIccProfileXml::ParseTag
* 
* Purpose: This will load from the indicated IO object and associate a tag
*  object to a tag directory entry.  Nothing happens if tag directory entry
*  is associated with a tag object.
* 
* Args: 
*  pNode - pointer to xmlNode object to parse from
* 
* Return: 
*  true - tag from node successfully parsed,
*  false - failure
*******************************************************************************
*/
bool CIccProfileXml::ParseTag(xmlNode *pNode, std::string &parseStr)
{
  xmlAttr *attr;
  
  if (pNode->type != XML_ELEMENT_NODE) {// || icXmlStrCmp(pNode->name, "Tag")) {
	  parseStr += "Invalid Tag Node: ";
	  parseStr += (const char *)pNode->name;
	  parseStr += "\n";
	  return false;
  }

  CIccTag *pTag = NULL;
  icSignature sigTag = (icSignature)0;

 // get the tag signature
  std::string nodeName = (icChar*) pNode->name;
  icTagTypeSignature sigType = icGetTypeNameTagSig (nodeName.c_str());
  
  if (sigType==icSigUnknownType){
	  xmlAttr *attr = icXmlFindAttr(pNode, "type");
	  sigType = (icTagTypeSignature)icGetSigVal((icChar*) icXmlAttrValue(attr));
  }

  CIccInfo info;

  // create a tag based on the signature
  pTag = CIccTag::Create(sigType);
  
  IIccExtensionTag *pExt;

  if (pTag && (pExt = pTag->GetExtension()) && !strcmp(pExt->GetExtClassName(), "CIccTagXml")) {
    CIccTagXml* pXmlTag = (CIccTagXml*)pExt;

	  if (pXmlTag->ParseXml(pNode->children, parseStr)) {
	    if ((attr=icXmlFindAttr(pNode, "reserved"))) {
	     sscanf(icXmlAttrValue(attr), "%u", &pTag->m_nReserved);
	    }
  	  
	    for (xmlNode *tagSigNode = pNode->children; tagSigNode; tagSigNode = tagSigNode->next) {
	 	    if (tagSigNode->type == XML_ELEMENT_NODE && !icXmlStrCmp(tagSigNode->name, "TagSignature")) {
			    sigTag = (icTagSignature)icGetSigVal((const icChar*)tagSigNode->children->content);
			    AttachTag(sigTag, pTag);
		    }
	    }	  
	  }
	  else {
	    parseStr += "Unable to Parse \"";
	    parseStr += info.GetTagTypeSigName(sigType);
	    parseStr += "\" (";
      parseStr += nodeName;
      parseStr += ") Tag\n";
	    return false;
	  }
  }
  else {
	  parseStr += "Invalid tag extension for \"";
	  parseStr += info.GetTagTypeSigName(sigType);
    parseStr += "\" (";
    parseStr += nodeName;
    parseStr += ") Tag\n";
	return false;
  }

  switch(sigTag) {
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
    if (pTag->IsMBBType())
      ((CIccMBB*)pTag)->SetColorSpaces(m_Header.colorSpace, m_Header.pcs);
    break;

  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
    if (pTag->IsMBBType())
      ((CIccMBB*)pTag)->SetColorSpaces(m_Header.pcs, m_Header.colorSpace);
    break;

  case icSigGamutTag:
    if (pTag->IsMBBType())
      ((CIccMBB*)pTag)->SetColorSpaces(m_Header.pcs, icSigGamutData);
    break;

  case icSigNamedColor2Tag:
    {
      if (pTag->GetType()==icSigNamedColor2Type) {
        ((CIccTagNamedColor2*)pTag)->SetColorSpaces(m_Header.pcs, m_Header.colorSpace);
      }
      else if (pTag->GetTagArrayType()==icSigNamedColorArray) {
        CIccArrayNamedColor *pAry = (CIccArrayNamedColor*)icGetTagArrayHandler(pTag);

        if (pAry) {
          pAry->SetColorSpaces(m_Header.pcs, m_Header.colorSpace, 
                               m_Header.spectralPCS, 
                               &m_Header.spectralRange, 
                               &m_Header.biSpectralRange);
        }
      }
    }

  }

  return true;
}


// entry function for converting xml to icc
bool CIccProfileXml::ParseXml(xmlNode *pNode, std::string &parseStr)
{  
  if (icXmlStrCmp(pNode->name, "IccProfile")) {
    return false;
  }

  xmlNode *hdrNode = icXmlFindNode(pNode->children, "Header");

  // parse header
  if (!hdrNode || !ParseBasic(hdrNode, parseStr))
    return false;

  // parse each tag
  xmlNode *tagNode = icXmlFindNode(pNode->children, "Tags");
  if (!tagNode)
    return false;

  for (tagNode = tagNode->children; tagNode; tagNode = tagNode->next) {
    if (tagNode->type == XML_ELEMENT_NODE) {
      if (!ParseTag(tagNode, parseStr))
        return false;
    }
  }

  return true;
}

// entry function for converting icc to xml
bool CIccProfileXml::LoadXml(const char *szFilename, const char *szRelaxNGDir, std::string *parseStr)
{  
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;

  /*parse the file and get the DOM */
  doc = xmlReadFile(szFilename, NULL, 0);

  if (doc == NULL) 
    return NULL;

  if (szRelaxNGDir && szRelaxNGDir[0]) {
    xmlRelaxNGParserCtxt* rlxParser;
    
    rlxParser = xmlRelaxNGNewParserCtxt (szRelaxNGDir);

    //validate the xml file
    if (rlxParser){
	    xmlRelaxNG* relaxNG = xmlRelaxNGParse(rlxParser);
	    if (relaxNG){
		    xmlRelaxNGValidCtxt* validCtxt = xmlRelaxNGNewValidCtxt(relaxNG);
		    if (validCtxt){
			    int result = xmlRelaxNGValidateDoc(validCtxt, doc);
			    if (result != 0){
				    printf("\nError: %d: '%s' is an invalid XML file.\n", result, szFilename);
				    return false;
			    }
		    }
		    else
		      return false;
	    }
	    else
		  return false;		  
    }
    else
	    return false;  
  }
   
  std::string my_parseStr;

  if (!parseStr)
    parseStr = &my_parseStr;

  *parseStr = "";

  /*Get the root element node */
  root_element = xmlDocGetRootElement(doc);

  bool rv = ParseXml(root_element, *parseStr);

  xmlFreeDoc(doc);

  return rv;
}
