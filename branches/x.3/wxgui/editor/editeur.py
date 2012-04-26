#!/usr/bin/env python
# generated by wxGlade 0.4.1 on Fri Jul 18 12:43:34 2008

"""-------------------------------------------------------------------------
 editeur, an editor class for Pinguino IDE

                         (c) 2008 Jean-Pierre MANDON <jp.mandon@gmail.com> 

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

        You should have received a copy of the GNU Lesser General Public
        License along with this library; if not, write to the Free Software
        Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------"""

# $Id: editeur.py,v beta 1 2008/09/06 15:03:00 jean-pierre mandon
# bug #01 replaced splitext by split in savefile
# $Id: editeur.py,v beta 6 2009/06/17 17:00:00 jean-pierre mandon
# full support of UTF8
# case sensitive help
# $Id: editeur.py,v beta 2 2010/02/15 15:03:00 jean-pierre mandon
# added function highlightline
# lexer modified

import wx
import os
import sys
import codecs
import wx.stc as stc
import keyword
from wxgui._ import _

faces = { 'helv' : 'Arial',
          'times': 'Times New Roman',
          'mono' : 'Courier New',
          'other': 'Comic Sans MS',
          'size' : 8,
          'size2': 8,
          }

class MyFileDropTarget(wx.FileDropTarget):
    def __init__(self, Open):
        wx.FileDropTarget.__init__(self)
        self.Open = Open

    def OnDropFiles(self, x, y, filenames):
        #self.window.AppendText("%d file(s) dropped at (%d,%d):\n" % (len(filenames), x, y))
        for file in filenames:
            self.Open(file)
            
            

########################################################################
class editor:

    #----------------------------------------------------------------------
    def __initEditor__(self):
        """Constructor"""
        self.onglet=[]
        self.stcpage=[]
        self.filename=[]
        self.sheetFunctions = []
        self.choiceFunctions = []
        self.line=-1
        self.inhibitChangeEvents = False
        self.notebookEditor.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.update_dockFiles)




    def focus(self):
        self.stcpage[self.notebookEditor.GetSelection()].SetFocus()
        return

    def EOF(self):
        if len(self.onglet)>0:
            if self.editeur.GetCurrentPos()==self.editeur.GetTextLength():
                return True
            else:
                return False
        else:
            return(False)

    def gotostart(self):
        self.editeur.GotoLine(self.editeur.LineFromPosition(0))
        return


    #----------------------------------------------------------------------
    def getLineOfText(self, toFind):
        if len(self.onglet)>0:
            self.editeur=self.stcpage[self.notebookEditor.GetSelection()]
            text = self.editeur.GetText()
            text = text.split("\n")
            return text.index(toFind)

    #----------------------------------------------------------------------
    def buildSheet(self, name):
        #ImageClose = os.path.join(sys.path[0], "wxgui", "resources", "close.png")
        p = wx.Panel(self.notebookEditor,-1, style = wx.NO_FULL_REPAINT_ON_RESIZE)
        self.onglet.append(p)
        
        stc = wx.stc.StyledTextCtrl(id=wx.NewId(),
                                    name='styledTextCtrl1', parent=p,  # pos=wx.Point(0, 35),
                                    size=wx.Size(-1, -1), 
                                    style=wx.SUNKEN_BORDER)

        stc.SetLexer(wx.stc.STC_LEX_CPP)

        stc.SetMargins(2,2)

        stc.SetMarginType(1, wx.stc.STC_MARGIN_NUMBER)
        stc.SetMarginWidth(1, 40)
        stc.SetMarginWidth(2, 10)

        stc.StyleSetSpec(wx.stc.STC_STYLE_LINENUMBER,"fore:#000000,back:#afc8e1ff,size:500")                

        stc.SetMarginSensitive(1, True)
        stc.SetMarginSensitive(2, True)

        stc.SetMarginType(3, wx.stc.STC_MARGIN_SYMBOL)
        stc.SetMarginMask(3, wx.stc.STC_MASK_FOLDERS)
        stc.SetMarginSensitive(3, True)
        stc.SetMarginWidth(3, 12)                

        stc.SetProperty("fold", "1")
        stc.SetProperty("tab.timmy.whinge.level", "1")

        stc.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEROPEN,    wx.stc.STC_MARK_BOXMINUS,          "white", "#808080")
        stc.MarkerDefine(wx.stc.STC_MARKNUM_FOLDER,        wx.stc.STC_MARK_BOXPLUS,           "white", "#808080")
        stc.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERSUB,     wx.stc.STC_MARK_VLINE,             "white", "#808080")
        stc.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERTAIL,    wx.stc.STC_MARK_LCORNER,           "white", "#808080")
        stc.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEREND,     wx.stc.STC_MARK_BOXPLUSCONNECTED,  "white", "#808080")
        stc.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEROPENMID, wx.stc.STC_MARK_BOXMINUSCONNECTED, "white", "#808080")
        stc.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERMIDTAIL, wx.stc.STC_MARK_TCORNER,           "white", "#808080")

        self.stcpage.append(stc)
        stc.SetMinSize(wx.Size(-1, -1))

        p.SetAutoLayout( True )
        p.Layout()
        
        boxSizer = wx.BoxSizer(orient=wx.VERTICAL)        
        boxSizer.Fit(p)
        boxSizer.SetSizeHints(p)
        p.SetSizer(boxSizer)        
        boxSizer.AddWindow(stc, 1, border=0, flag=wx.EXPAND|wx.ADJUST_MINSIZE)


        self.notebookEditor.AddPage(p,os.path.split(name)[1])
        self.SendSizeEvent()
#        self._mgr.Update()
        self.panelEditor.Layout()
        self.sheetFunctions.append({})      

    #----------------------------------------------------------------------
    def New(self, name):
        """ open a new tab """
        
        self.background.Hide()
        self.notebookEditor.Show()
        self.buildSheet(name)
        self.updateIDE()
        self.notebookEditor.ChangeSelection(len(self.onglet)-1) ### Set ~ ChangeSelection -- bjoernp
#        self.notebookEditor.SetSelection(len(self.onglet)-1) ### Set ~ ChangeSelection -- bjoernp
#        print len(self.onglet)-1
#        print self.notebookEditor.GetSelection()
        newIdx = self.notebookEditor.GetSelection()
        self.stcpage[newIdx].Bind(stc.EVT_STC_MODIFIED,self.OnChange)
        self.stcpage[newIdx].Bind(wx.EVT_KEY_DOWN, self.OnKeyDown)
        self.stcpage[newIdx].Bind(wx.EVT_LEFT_UP, self.onclick)
        self.stcpage[newIdx].Bind(wx.EVT_RIGHT_UP, self.onclick)
        #self.filename.append(os.getcwd()+"/"+name+".pde")        
        self.filename.append(name+".pde")
        #self.filename.append(unicode(name, 'utf8')+".pde")
        self.seteditorproperties(self.reservedword, self.rw)
        #x,y=self.GetSize()
        #self.stcpage[self.notebookEditor.GetSelection()].SetSize((x,y-20))
        self.editeur=self.stcpage[newIdx]
        self.editeur.Bind(wx.EVT_CONTEXT_MENU, self.contexMenuTools)
        self.editeur.Bind(wx.EVT_KEY_UP, self.keyEvent)
        #self.editeur.Bind(wx.stc.EVT_STC_CHARADDED, self.GetCharEvent)
        #self.editeur.Bind(wx.stc.EVT_STC_MODIFIED, self.keyEvent)        
        #self.editeur.Bind(wx.stc.EVT_STC_AUTOCOMP_SELECTION, self.inserted)
        self.editeur.Bind(wx.stc.EVT_STC_MODIFIED, self.updateStatusBar)
        self.editeur.Bind(wx.stc.EVT_STC_MODIFIED, self.update_dockFiles)
        self.editeur.Bind(wx.EVT_LEFT_UP, self.updateStatusBar)
        self.editeur.Bind(wx.EVT_LEFT_UP, self.OnLeftCklick)
        self.editeur.Bind(wx.EVT_KEY_UP, self.updateStatusBar)
        self.editeur.Bind(wx.stc.EVT_STC_MARGINCLICK, self.OnMarginClick)
        #self.editeur.Bind(wx.stc.EVT_STC_DO_DROP, self.OnDrop)  
        #self.editeur.Bind(wx.stc.EVT_STC_dr, self.OnDrag)        
        self.editeur.Bind(wx.stc.EVT_STC_SAVEPOINTLEFT, self.OnSavepointLeft)  
        self.editeur.Bind(wx.stc.EVT_STC_SAVEPOINTREACHED, self.OnSavepointReached)
        
        self.editeur.MarkerDefine (2, wx.stc.STC_MARK_SHORTARROW, "white", "white")

        self.insertSnippet("Insert Date {snippet}")
        self.editeur.GotoLine(self.editeur.LineCount)
        self.editeur.AppendText("\n\n")
        self.editeur.GotoLine(self.editeur.LineCount)
        self.insertSnippet("Bare Minimum {snippet}")
        self.update_dockFiles()
        #self._mgr.Update()
        
        dt = MyFileDropTarget(self.Open)
        self.editeur.SetDropTarget(dt)        


    #----------------------------------------------------------------------
    def updatenotebook(self):
        if self.notebookEditor.PageCount == 0:
            self.notebookEditor.Hide()
            self.background.Show()
        else:
            self.background.Hide()
            self.notebookEditor.Show()


# modified by r.blanchot 31/10/2010, 01/06/2011

    def OpenDialog(self,type,extension):
        """ Open Dialog and load file in a new editor """
        
        try: defaultDir=os.path.split(self.filename[self.notebookEditor.GetSelection()])[0]
        except: defaultDir = sys.path[0]
        
        opendlg = wx.FileDialog(self,
                                message=_("Choose a file"),
                                #defaultDir=sys.path[0],
                                defaultDir=defaultDir, 
                                defaultFile="",
                                wildcard=type+" (*"+extension+")|*"+extension,
                                style=wx.OPEN  | wx.CHANGE_DIR)

        if opendlg.ShowModal() == wx.ID_OK:
            paths = opendlg.GetPaths()
            for path in paths:
                self.Open(path)

# added by r.blanchot 31/10/2010

    def Open(self, path):
        """ Open file in a new editor """ 
        file = os.path.basename(path)
        # --- file history --- added by r.blanchot 03/11/2010
        #filehistory.AddFileToHistory(path)
        #filehistory.Save(config)
        #config.Flush()
        #
        alloaded=-1
        directory,extension = os.path.splitext(path)
        #for i in range(len(self.stcpage)):
            #try: file = unicode(file).encode("utf-8")
            #except: pass
            #if file == self.notebookEditor.GetPageText(i):
                #alloaded=i
        #if alloaded!=-1:
        if path in self.filename:
            
            dlg = wx.MessageDialog(self,
                                   _("File is already opened, reload it ?"), _("Warning")+"!",
                                   wx.YES_NO | wx.ICON_WARNING)
            result=dlg.ShowModal()
            dlg.Destroy()                        
            if (result==wx.ID_NO):
                return
            else:
                self.inhibitChangeEvents = True
                self.stcpage[alloaded].ClearAll()
                fichier=open(path,'r')
                #for line in fichier:
                #    self.stcpage[alloaded].addtext(line)
                self.stcpage[alloaded].SetText(fichier.read())
                fichier.close()
                self.stcpage[alloaded].SetSavePoint()
                self.notebookEditor.SetSelection(alloaded)
                self.inhibitChangeEvents = False
                return
        self.inhibitChangeEvents = True
        self.New(file.replace(extension,""))
                
        pageIdx = self.notebookEditor.GetSelection()
        self.stcpage[pageIdx].ClearAll()
        self.filename[pageIdx]=path
        fichier=codecs.open(path,'r','utf8')
        #for line in fichier:
        #    self.stcpage[pageIdx].AddText(line)
        self.stcpage[pageIdx].SetText(fichier.read())
        fichier.close()
        self.notebookEditor.SetPageText(pageIdx,file.replace(extension,""))
        self.gotostart()
        self.update_dockFiles()
        #self.notebookEditor.Update()
        self.stcpage[pageIdx].EmptyUndoBuffer()
        #self.stcpage[pageIdx].SetSavePoint()
        self.inhibitChangeEvents = False
        self.SendSizeEvent()
        
        self.addFile2Recent(path)
        
        
                
    def Save(self,type,extension):
        """save the content of the editor to filename""" 
        if len(self.onglet)>0: 
            pageIdx = self.notebookEditor.GetSelection()
            path=self.filename[pageIdx]
            # directory,extension=os.path.splitext(path) bug #01 2008-09-06
            directory,extension=os.path.split(path)
            file=os.path.basename(path)  
            filedlg = wx.FileDialog(
                self, 
                message=_("Save file as")+" ...", 
                defaultDir=directory, 
                defaultFile=file, 
                wildcard=type+" (*"+extension+")|*"+extension,
                style=wx.SAVE)
            filedlg.SetFilterIndex(2)
            if filedlg.ShowModal() == wx.ID_OK:
                path = filedlg.GetPath() 
            else:
                return
            if (path!=""):
                if os.path.exists(path):
                    dlg = wx.MessageDialog(self,
                                           _("File already exist, Overwrite it ?"), _("Warning")+"!",
                                           wx.YES_NO | wx.ICON_WARNING
                                           )
                    result=dlg.ShowModal()
                    dlg.Destroy()                        
                    if (result!=wx.ID_YES):
                        return 0         
            self.filename[pageIdx]=path
            directory,extension=os.path.splitext(path)
            file=os.path.basename(path)                
            self.notebookEditor.SetPageText(pageIdx,file.replace(extension,""))
            fichier=codecs.open(path,'w','utf8')                        
            for i in range(0,self.stcpage[pageIdx].GetLineCount()):
                fichier.writelines(unicode(self.stcpage[pageIdx].GetLine(i)))
            fichier.close()
            self.stcpage[pageIdx].SetSavePoint()
            return

    #def SaveDirect(self):
        #""" Save file without dialog box """
        #if len(self.onglet)>0: 
            #path=self.filename[self.notebookEditor.GetSelection()]
            #fichier=codecs.open(path,'w','utf8')
            #for i in range(0,self.stcpage[self.notebookEditor.GetSelection()].GetLineCount()):
                #fichier.writelines(self.stcpage[self.notebookEditor.GetSelection()].GetLine(i))
            #fichier.close()
            #if self.notebookEditor.GetPageText(self.notebookEditor.GetSelection())[0]=="*":
                #chaine=self.notebookEditor.GetPageText(self.notebookEditor.GetSelection())
                #chaine=chaine[1:len(chaine)]
                #self.notebookEditor.SetPageText(self.notebookEditor.GetSelection(),chaine)
            #return
        #return

    def highlightline(self,line,color):
        """highlight a line """
        self.stcpage[self.notebookEditor.GetSelection()].GotoLine(line)
        self.stcpage[self.notebookEditor.GetSelection()].SetCaretLineBack(color)
        self.stcpage[self.notebookEditor.GetSelection()].SetCaretLineVisible(1)
        self.Refresh()
        return

    def CloseTab(self):
        """ close the current tab """
        if len(self.onglet) <= 0: return False
        
        pageIdx = self.notebookEditor.GetSelection()
        if self.notebookEditor.GetPageText(pageIdx)[0]=="*":
            dlg = wx.MessageDialog(self,
                                   _("Save file ?"), _("Warning")+"!",
                                   wx.YES_NO | wx.ICON_WARNING
                                   )
            result=dlg.ShowModal()
            dlg.Destroy()                        
            if (result==wx.ID_YES):
                self.Save("Pde File","pde")  
        self.filename.remove(self.filename[pageIdx])
        self.onglet.remove(self.onglet[pageIdx])
        self.stcpage.remove(self.stcpage[pageIdx])
        self.notebookEditor.DeletePage(pageIdx)
        self.sheetFunctions.remove(self.sheetFunctions[pageIdx])
        #self.choiceFunctions.remove(self.choiceFunctions[page])
        if pageIdx > 0:
            self.notebookEditor.SetSelection(pageIdx-1)
            self.notebookEditor.Update()
        return True


    def GetPath(self):
        """ return the complete path of file """
        if self.notebookEditor.GetSelection()!=-1:
            return self.filename[self.notebookEditor.GetSelection()]
        else:
            return -1

    def SetModified(self,status):
        pageIdx =  self.notebookEditor.GetSelection()
        modSet = self.notebookEditor.GetPageText(pageIdx)[0] == "*"
        if status == False:
            if modSet:
                self.notebookEditor.SetPageText(pageIdx,self.notebookEditor.GetPageText(pageIdx)[1:])
        else:
            if not modSet: 
                self.notebookEditor.SetPageText(pageIdx,"*"+self.notebookEditor.GetPageText(pageIdx))

    def OnSavepointReached(self,event):
        self.SetModified(False)

    def OnSavepointLeft(self,event):
        self.SetModified(True)

    def OnChange(self,event):
        """ modified editor window event """
        #event.Skip()
        #if self.inhibitChangeEvents:
        #    return
        #modType = event.GetModificationType()
        #if (stc.STC_PERFORMED_USER & modType) == 0: 
        #    return
        #self.UpdateModified()

        if self.stcpage[self.notebookEditor.GetSelection()].GetCaretLineVisible()==True:
            self.stcpage[self.notebookEditor.GetSelection()].SetCaretLineVisible(0)
        
    def onclick(self,event):
        if self.stcpage[self.notebookEditor.GetSelection()].GetCaretLineVisible()==True:
            self.stcpage[self.notebookEditor.GetSelection()].SetCaretLineVisible(0)
        event.Skip()

    def OnKeyDown(self,event):
        """ keydown windows event """

        global rw
        localkw=[]
        k_code = event.GetKeyCode()

        # automatic indentation
        if k_code == wx.WXK_RETURN and self.stcpage[self.notebookEditor.GetSelection()].AutoCompActive()!=1:
            line=self.stcpage[self.notebookEditor.GetSelection()].GetCurrentLine()
            text = self.stcpage[self.notebookEditor.GetSelection()].GetTextRange(self.stcpage[self.notebookEditor.GetSelection()].PositionFromLine(line), \
                                                                            self.stcpage[self.notebookEditor.GetSelection()].GetCurrentPos())
            if text.strip() == u'':
                self.stcpage[self.notebookEditor.GetSelection()].AddText('\r\n' + text)
                self.stcpage[self.notebookEditor.GetSelection()].EnsureCaretVisible()
                return
            indent = self.stcpage[self.notebookEditor.GetSelection()].GetLineIndentation(line)
            i_space = indent / self.stcpage[self.notebookEditor.GetSelection()].GetTabWidth()
            ndent = u'\r\n' + u'\t' * i_space
            self.stcpage[self.notebookEditor.GetSelection()].AddText(ndent + \
                                                                ((indent - (self.stcpage[self.notebookEditor.GetSelection()].GetTabWidth() * i_space)) * u' '))
            self.stcpage[self.notebookEditor.GetSelection()].EnsureCaretVisible()
            return

        # self completion
        if k_code == 32 and event.ControlDown():
            self.stcpage[self.notebookEditor.GetSelection()].AutoCompSetIgnoreCase(True)  # so this needs to match
            self.stcpage[self.notebookEditor.GetSelection()].AutoCompSetAutoHide(True)        # auto hide list when nothing matches
            self.stcpage[self.notebookEditor.GetSelection()].AutoCompShow(0, " ".join(keywordhelp))
            event.Skip()
        event.Skip()        

    def seteditorproperties(self,reservedword,rw):
        """ set the layout,keywords and layout for syntax"""
        global keywordhelp
        keywordhelp=rw
        #self.stcpage[self.notebookEditor.GetSelection()].SetLexer(stc.STC_LEX_CPP)
        kw = keyword.kwlist                
        for i in range(0,len(reservedword)):
            kw.append(reservedword[i])

        font = wx.Font(10, wx.TELETYPE, wx.NORMAL, wx.NORMAL, True)        
        self.stcpage[self.notebookEditor.GetSelection()].SetKeyWords(0, " ".join(keyword.kwlist))                 
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_PREPROCESSOR, "face:%s,size:10,fore:#d36820" %  font.GetFaceName())                
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_DEFAULT, "face:%s,size:10,fore:#000000" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_COMMENT, "face:%s,size:10,fore:#c81818" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_COMMENTLINE, "face:%s,size:10,fore:#007F00" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_NUMBER, "face:%s,size:10,fore:#ff0000" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_STRING, "italic,face:%s,size:10,fore:#7f0000" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_CHARACTER, "face:%s,size:10,fore:#cc0000" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_WORD, "face:%s,size:10,fore:#0C36F0" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_OPERATOR, "face:%s,size:10" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_IDENTIFIER, "face:%s,size:10" % font.GetFaceName())
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_STRINGEOL, "fore:#000000,back:#E0C0E0,eol,size:%(size)d" % faces)
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_COMMENTDOC, "fore:#5e5ef1,size:%(size)d" % faces)
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_COMMENTLINEDOC,  "fore:#007F00,size:%(size)d" % faces)
        self.stcpage[self.notebookEditor.GetSelection()].StyleSetSpec(stc.STC_C_GLOBALCLASS, "fore:#7F7F7F,size:%(size)d" % faces)
        self.stcpage[self.notebookEditor.GetSelection()].SetCaretForeground("BLACK")
        self.stcpage[self.notebookEditor.GetSelection()].SetCaretWidth(1)
        self.stcpage[self.notebookEditor.GetSelection()].SetBackSpaceUnIndents(True)
        #self.stcpage[self.notebookEditor.GetSelection()].SetMarginWidth(0, 30)
        #self.stcpage[self.notebookEditor.GetSelection()].SetMarginType(0,stc.STC_MARGIN_NUMBER)
        self.stcpage[self.notebookEditor.GetSelection()].UsePopUp(1)

        return
    
    
    
