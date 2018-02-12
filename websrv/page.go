package main

import (
	"text/template"
	"io"
)

type Attribute struct {
	Name  string
	Value string
}

type Element struct {
	Type       string
	Id         string
	Class      []string
	Attributes []Attribute
	Children   []Element
	Content    string
	Empty      bool
}

type PageHead struct {
	Title       string
	Meta        []Attribute
	StyleSheets []string
	Scripts     []string
}

type PageBody struct {
	Header []Element
	Main   []Element
	Footer []Element
}

type Page struct {
	Head PageHead
	Body PageBody
}

var pageTmpl = template.Must(template.New("page").ParseFiles("app/page.html"))

func (p *Page) Build(w io.Writer) error {
	return pageTmpl.Execute(w, p)
}
